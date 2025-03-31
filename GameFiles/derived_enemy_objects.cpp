// Definitions for every derived enemy object

#include "derived_enemy_objects.h"
#include "projectile_game_object.h"
#include <iostream>

namespace game {


	/*
	* GunnerEnemy Definitions
	*/
	GunnerEnemy::GunnerEnemy(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: EnemyGameObject(position, geom, shader, texture), origin_pos(position) {
		scale_ = glm::vec2(1.1f);
		orbit_angle = 0;
		enemy_health = 20;
		enemy_damage = 40;
		bullet_timer.Start(BULLET_TIMER);
	}
	void GunnerEnemy::Update(double delta_time) {
		EnemyGameObject::Update(delta_time);

		// move the origin point towards the player
		origin_pos += velocity_ * GUNNER_SPEED * static_cast<float>(delta_time) * 0.2f;

		// rotate around a the origin position (via parametric equations)
		orbit_angle += delta_time * ORBIT_SPEED;
		position_.x = origin_pos.x + ORBIT_RADIUS * cos(orbit_angle);
		position_.y = origin_pos.y + ORBIT_RADIUS * sin(orbit_angle);
	}

	bool GunnerEnemy::IsFinished() {
		if (bullet_timer.Finished()) {
			bullet_timer.Start(BULLET_TIMER);
			return true;
		}
		else {
			return false;
		}
	}

	/*
	* ChaserEnemy Definitions
	*/
	ChaserEnemy::ChaserEnemy(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: EnemyGameObject(position, geom, shader, texture) {
		scale_ = glm::vec2(0.9f);
		enemy_health = 10;
		enemy_damage = 20;
	}
	void ChaserEnemy::Update(double delta_time) {
		EnemyGameObject::Update(delta_time);

		// basic pursuit method
		position_.x += velocity_.x * CHASER_SPEED * delta_time;
		position_.y += velocity_.y * CHASER_SPEED * delta_time;
	}


	/*
	* KamikazeEnemy Definitions
	*/
	KamikazeEnemy::KamikazeEnemy(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: EnemyGameObject(position, geom, shader, texture) {
		scale_ = glm::vec2(0.6f);
		enemy_health = 3;
		enemy_damage = 30;
		full_send = false;
	}
	void KamikazeEnemy::Update(double delta_time) {
		EnemyGameObject::Update(delta_time);

		// basic pursuit method
		position_.x += velocity_.x * KAMIKAZE_SPEED * delta_time;
		position_.y += velocity_.y * KAMIKAZE_SPEED * delta_time;
	}

	void KamikazeEnemy::UpdateTarget(GameObject* obj) {
		// float distance = glm::length(target_pos - position_);
		target_pos = obj->GetPosition() + obj->GetVelocity() * 1.5f;
		velocity_ = glm::normalize(target_pos - position_);
		full_send = false;
	}

} // namespace game
