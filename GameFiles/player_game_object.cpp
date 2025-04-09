// PlayerGameObject class definitions

#include "player_game_object.h"

namespace game {

	/*** Constructor ***/
	PlayerGameObject::PlayerGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: GameObject(position, geom, shader, texture) {

		// initialize default values
		health = PLAYER_INIT_HP;
		max_health = health;
		regen_cd_time = INIT_REGEN_CD;
		regen_step_amount = INIT_REGEN_AMOUNT;

		angle_ = 0;
		target_angle = 0;
		points = 0;
		weapon = (0, 0, 0, 0, 0, 0);
		weapon_id = 0;

		accel_force = PLAYER_INIT_ACCEL;
		max_speed = INIT_PLAYER_MAX_SPEED;

		double_points = false;
		bullet_boost = false;
		cold_shock = false;

		armor_plating = false;
		regen_coating = false;
		nitro_infuse = false;
		celestial_augment = false;
		
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
			if (nitro_infuse) {
				max_speed = 6.4f;
			}
			else {
				max_speed = INIT_PLAYER_MAX_SPEED;
			}
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
				health += regen_step_amount;

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
				regen_cd.Start(regen_cd_time);
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
			return weapon->GetDamage() * 3;
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


	/*** Handle knockback effect ***/
	void PlayerGameObject::ApplyKnockback(glm::vec3& direction, int damage) {
		TakeDamage(damage / 2);
		SetVelocity(direction);
		max_speed = 12.0f;
		knockback_cooldown.Start(0.2);
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

	
	/*** Handle enabling the armor-plating upgrade ***/
	void PlayerGameObject::EnableArmorPlating(void) {
		armor_plating = true;
		max_health = 150; // 50hp increase to max health
		health *= 1.5;
	}


	/*** Handle enabling the regen coating upgrade ***/
	void PlayerGameObject::EnableRegenCoating(void) {
		regen_coating = true;
		regen_cd_time = 2.5f; // reduce regen cd by 1.5s
		regen_step_amount = 2; // regens 20hp/s instead of 10hp/s
	}


	/*** Handle enabling the nitro-infuse upgrade ***/
	void PlayerGameObject::EnableNitroInfuse(void) {
		nitro_infuse = true;
		accel_force = 30.0f; // 1.5x the acceleration
		max_speed = 6.4f;    // 2.2u/s max speed increase
	}


	/*** Handle enabling the celestial augment upgrade ***/
	void PlayerGameObject::EnableCelestialAugment(void) {
		
		if (!celestial_augment) {
			celestial_augment = true;

			Weapon* weapon = GetWeapon();

			// BIG WEAPON BUFFS, double damage, faster rpm, becomes auto if not already
			weapon->SetDamage(weapon->GetDamage() * 2.0);
			weapon->SetFiringCooldown(weapon->GetFiringCooldown() * 0.8f);
			weapon->SetSemiAuto(false);
		}

		// NOTE: only applies to current gun... if the player swaps weapons, returns to default values.
	}

}
