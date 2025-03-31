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
		enemy_health = 1;
		enemy_damage = 0;
		target_pos = glm::vec3(0.0f);
	}


	/*** Base Update function, ensures enemy faces the player ***/
	void EnemyGameObject::Update(double delta_time) {
		glm::vec3 aim_line = target_pos - position_;
		angle_ = atan2(aim_line.y, aim_line.x) - (glm::pi<float>() / 2);
	}


	/*** Updates the target for chase movement ***/
	void EnemyGameObject::UpdateTarget(GameObject* obj) {
		target_pos = obj->GetPosition();
		velocity_ = glm::normalize(target_pos - position_);
	}

} // namespace game
