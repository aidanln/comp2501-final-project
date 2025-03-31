// EnemyGameObject class definitions

#include "enemy_game_object.h"

#define GLM_FORCE_RADIANS

namespace game {

	/*** Constructor ***/
	EnemyGameObject::EnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: GameObject(position, geom, shader, texture) {
		// initialize to default values
		angle_ = 0;
		exploded = false;
	}


	/*** Base Update function, ensures enemy faces the player ***/
	void EnemyGameObject::Update(double delta_time) {
		glm::vec3 aim_line = target_pos - position_;
		angle_ = atan2(aim_line.y, aim_line.x) - (glm::pi<float>() / 2);
	}


	/*** Updates the target for chase movement ***/
	void EnemyGameObject::UpdateTarget(const glm::vec3& position) {
		target_pos = position;

		// calculate direction vector and distance to target
		float x_dir = target_pos.x - position_.x;
		float y_dir = target_pos.y - position_.y;
		float distance = std::sqrt((x_dir * x_dir) + (y_dir * y_dir));

		// avoid "divide by 0" errors
		if (!(distance <= 0.0001f)) {
			// set velocity vector as normalized direction vectors
			velocity_.x = x_dir / distance;
			velocity_.y = y_dir / distance;
		}
	}

} // namespace game
