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
		
		// initialize health timers
		i_frames_timer.Start(0.0);
		regen_cd.Start(0.0);
		regen_step.Start(0.0);
	}


	/*** Player-Specific Update function ***/
	void PlayerGameObject::Update(double delta_time) {
		float dt = static_cast<float>(delta_time);

		// Update velocity based on acceleration
		velocity_ += acceleration_ * dt;

		// Ensure velocity doesn't exceed max speed
		if (glm::length(velocity_) > PLAYER_MAX_SPEED) {
			velocity_ = glm::normalize(velocity_) * PLAYER_MAX_SPEED;
		}

		// Update position based on velocity, then decelerate
		position_ += velocity_ * dt;
		velocity_ /= 1.01 + (dt * 3);

		// Update rotation using lerp with the target angle, ensures smooth motion
		angle_ = LerpAngle(angle_, target_angle, 0.1f);

		// Regenerate health, provided the cooldown is finished and we are not already at max
		if (regen_cd.Finished() && health < max_health) {

			// Use "steps" to regenerate, i.e. regen one time once a step has finished
			if (regen_step.Finished()) {
				regen_step.Start(REGEN_STEP_CD);
				health += REGEN_AMOUNT;

				// clamp health to never exceed max_health
				if (health > max_health) {
					health = max_health;
				}

				// debug, needed until HUD is implemented
				std::cout << "Player HP: " << health << " -> Regenerated " << REGEN_AMOUNT << " Health." << std::endl;
			}
		}

		// Check power-up timers, if expired, reset states back to false
		if (dp_timer.FinishedAndStop()) {
			double_points = false;
		}
		if (bb_timer.FinishedAndStop()) {
			bullet_boost = false;
			Weapon* weapon = GetWeapon();
			weapon->SetDamage(weapon->GetDamage() / 2);
		}
		if (cs_timer.FinishedAndStop()) {
			cold_shock = false;
		}
	}


	/*** Longer erase timer, as this only plays during a game over ***/
	void PlayerGameObject::StartEraseTimer(void) {
		erase_timer_.Start(5.0f);
	}


	/*** Decrease health by param damage ***/
	void PlayerGameObject::TakeDamage(int recieved_dmg) {

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

			// debug, needed until HUD is implemented
			std::cout << "Player HP: " << health << " -> Took " << recieved_dmg << " Damage." << std::endl;
		}
	}


	/*** Add points to the player member var ***/
	void PlayerGameObject::AddPoints(int amount) {
		if (double_points) {
			points += amount * 2;
		}
		else {
			points += amount;
		}
		
	}


	/*** Handle Double Points state and timer ***/
	void PlayerGameObject::SetDoublePoints(bool tf) { 
		double_points = tf;
		if (tf) {
			dp_timer.Start(POWER_UP_DURATION);
		} 
	}


	/*** Handle Bullet Boost state and timer ***/
	void PlayerGameObject::SetBulletBoost(bool tf) {
		bullet_boost = tf;
		if (tf) {

			// avoid multiplying multiple times
			if (!bb_timer.IsRunning()) {
				Weapon* weapon = GetWeapon();
				weapon->SetDamage(weapon->GetDamage() * 2);
			}

			bb_timer.Start(POWER_UP_DURATION);
		}
	}


	/*** Handle Cold Shock state and timer ***/
	void PlayerGameObject::SetColdShock(bool tf) {
		cold_shock = tf;
		if (tf) {
			cs_timer.Start(POWER_UP_DURATION);
		}
	}

}
