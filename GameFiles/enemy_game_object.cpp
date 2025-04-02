// EnemyGameObject class definitions

#include "enemy_game_object.h"

#define GLM_FORCE_RADIANS

namespace game {

	/*** Constructor ***/
	EnemyGameObject::EnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: GameObject(position, geom, shader, texture) {
		// initialize to default values
		health = 1;
		damage = 0;
		exploded = false;
		target_angle = 0.0f;
		target_pos = glm::vec3(0.0f);
		
	}


	/*** Base Update function, ensures enemy faces the player, smooth rotation ***/
	void EnemyGameObject::Update(double delta_time) {
		glm::vec3 aim_line = target_pos - position_;
		target_angle = atan2(aim_line.y, aim_line.x) - (HALF_PI);
		angle_ = LerpAngle(angle_, target_angle, 0.05f);
	}


	/*** Base UpdateTarget function, updates the target_pos and adjusts velocity accordingly ***/
	void EnemyGameObject::UpdateTarget(GameObject* obj) {
		target_pos = obj->GetPosition();
		velocity_ = glm::normalize(target_pos - position_);
	}


	/*** Subtract param damage from health ***/
	void EnemyGameObject::TakeDamage(int recieved_dmg) {
		if (health > 0) {
			health -= recieved_dmg;
		}
		else if (health < 0) {
			health = 0;
		}
	}

} // namespace game
