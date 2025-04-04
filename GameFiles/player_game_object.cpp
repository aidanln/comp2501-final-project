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
		
		// start health timers
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

}
