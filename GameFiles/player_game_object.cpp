// PlayerGameObject class definitions

#include "player_game_object.h"

namespace game {

	/*** Constructor ***/
	PlayerGameObject::PlayerGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: GameObject(position, geom, shader, texture) {

		// initialize default values
		health = PLAYER_INIT_HP;
		max_health = health;
		angle_ = 0;
		target_angle = 0;
		points = 0;
		weapon = (0, 0, 0, 0, 0, 0);
		double_points = false;
		bullet_boost = false;
		cold_shock = false;
		max_speed = INIT_PLAYER_MAX_SPEED;
		
		// initialize health timers
		i_frames_timer.Start(0.0);
		regen_cd.Start(0.0);
		regen_step.Start(0.0);
	}


	/*** Player-Specific Update function ***/
	void PlayerGameObject::Update(double delta_time) {
		float dt = static_cast<float>(delta_time);
		// if done with knockback, set it back to normal speed
		if (knockback_cooldown.FinishedAndStop()) {
			max_speed = INIT_PLAYER_MAX_SPEED;
		}

		// Update velocity based on acceleration
		velocity_ += acceleration_ * dt;

		// Ensure velocity doesn't exceed max speed
		if (glm::length(velocity_) > max_speed) {
			velocity_ = glm::normalize(velocity_) * max_speed;
		}

		// Update position based on velocity, then decelerate
		position_ += velocity_ * dt;
		velocity_ /= 1.001 + (dt * 3); // weird

		// Clamp player position to ensure no OOB movement is possible
		position_.x = glm::clamp(position_.x, -PLAYER_X_BOUND, PLAYER_X_BOUND);
		position_.y = glm::clamp(position_.y, -PLAYER_Y_BOUND, PLAYER_Y_BOUND);

		// Update rotation using lerp with the target angle, ensures smooth motion
		angle_ = LerpAngle(angle_, target_angle, 0.1f);

		// Regenerate health, provided the cooldown is finished and we are not already at max
		if (regen_cd.Finished() && health < max_health) {

			// use "steps" to regenerate, i.e. regen one time once a step has finished
			if (regen_step.Finished()) {
				regen_step.Start(REGEN_STEP_CD);
				health += REGEN_AMOUNT;

				// clamp health to never exceed max_health
				if (health > max_health) {
					health = max_health;
				}
			}
		}

		// Check power-up timers, if expired, reset states back to false
		if (dp_timer.FinishedAndStop()) {
			double_points = false;
		}
		if (bb_timer.FinishedAndStop()) {
			bullet_boost = false;
		}
		if (cs_timer.FinishedAndStop()) {
			cold_shock = false;
		}
	}


	/*** Longer erase timer than default, as this only plays during a game over ***/
	void PlayerGameObject::StartEraseTimer(void) {
		erase_timer_.Start(5.0f);
	}


	/*** Decrease health by param damage ***/
	bool PlayerGameObject::TakeDamage(int recieved_dmg) {

		// Ensure damage isn't taken during invincibility frames
		if (i_frames_timer.Finished() && health > 0) {
			health -= recieved_dmg;

			// enters if-statement if dead, clamp health to never go below 0
			if (health < 0) {
				health = 0;
			}

			// start associated timers if not dead
			else {
				regen_cd.Start(REGEN_CD);
				i_frames_timer.Start(INVINCIBILITY_DURATION);
			}

			// indicate damage was taken
			SpendPoints(recieved_dmg * 5);
			if (points < 0) {
				points = 0;
			}
			return true;
		}

		// indicate no damage taken
		return false;
	}


	/*** Get the player's current damage, considers bullet boost flag ***/
	int PlayerGameObject::GetDamage(void) {
		if (bullet_boost) {
			return weapon->GetDamage() * 2;
		}
		else {
			return weapon->GetDamage();
		}
	}


	/*** Add points to the player member var, considers double points flag ***/
	void PlayerGameObject::AddPoints(int amount) {
		if (double_points) {
			points += amount * 2;
		}
		else {
			points += amount;
		}
	}


	/*** Handle Double Points state and timer ***/
	void PlayerGameObject::EnableDoublePoints(void) { 
		double_points = true;
		dp_timer.Start(POWER_UP_DURATION);
	}


	/*** Handle Bullet Boost state and timer ***/
	void PlayerGameObject::EnableBulletBoost(void) {
		bullet_boost = true;
		bb_timer.Start(POWER_UP_DURATION);
	}


	/*** Handle Cold Shock state and timer ***/
	void PlayerGameObject::EnableColdShock(void) {
		cold_shock = true;
		cs_timer.Start(POWER_UP_DURATION);
	}

	/*** Handle knockback effect ***/
	void PlayerGameObject::ApplyKnockback(glm::vec3& direction, int damage) {
		TakeDamage(damage/4);
		SetVelocity(direction);
		// std::cout << velocity_.x << ", " << velocity_.y << std::endl;
		max_speed = 12.0f;
		knockback_cooldown.Start(0.2);
	}
}
