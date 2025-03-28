// EnemyGameObject class definitions

#include "enemy_game_object.h"

#define GLM_FORCE_RADIANS

namespace game {

	/*** Constructor ***/
	EnemyGameObject::EnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: GameObject(position, geom, shader, texture), origin_pos(position) {
		// initialize to default values
		angle_ = glm::pi<float>() / 2.0f;
		target_update_timer.Start(2.0f); // update target every 2 seconds
		orbit_angle = 0;
		state = 0;
		exploded = false;
	}


	/*** Update function for moving the enemy around ***/
	void EnemyGameObject::Update(double delta_time) {

		// handle movement if not exploded
		if (!exploded) {
			glm::vec3 new_position;

			// handle "Patrolling" movement
			if (!state) {
				// update the angle (based on delta_time), use parametric equations to calculate x,y
				orbit_angle += delta_time * 1.5;
				new_position.x = origin_pos.x + ORBIT_RADIUS * cos(orbit_angle);
				new_position.y = origin_pos.y + ORBIT_RADIUS * sin(orbit_angle);
				SetPosition(new_position);

			}

			// handle "Intercepting" movement
			else {
				position_.x += velocity_.x * ENEMY_SPEED * delta_time;
				position_.y += velocity_.y * ENEMY_SPEED * delta_time;
			}
		}
	}


	/*** Updates the target for chase movement ***/
	void EnemyGameObject::UpdateTarget(const glm::vec3& position) {
		target_pos = position;

		// calculate direction vector and distance to target
		float x_dir = target_pos.x - position_.x;
		float y_dir = target_pos.y - position_.y;
		float distance = std::sqrt((x_dir * x_dir) + (y_dir * y_dir));

		// avoid "divide by 0" errors
		if (!(distance <= 0.0000001f)) {
			// set velocity vector as normalized direction vectors
			velocity_.x = x_dir / distance;
			velocity_.y = y_dir / distance;
		}
	}


	/*** Checks if the 2 second target update interval is done ***/ 
	bool EnemyGameObject::TargetUpdateCheck() {
		if (target_update_timer.Finished()) {
			target_update_timer.Start(2.0f);
			return true;
		}
		return false;
	}

} // namespace game
