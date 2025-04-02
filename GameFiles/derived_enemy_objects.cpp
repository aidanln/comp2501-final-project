// Definitions for every derived enemy object

#include "derived_enemy_objects.h"

namespace game {

	/***************************/
	/* GunnerEnemy Definitions */
	/***************************/

	/*** Constructor, initializes Gunner-specific defaults ***/
	GunnerEnemy::GunnerEnemy(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: EnemyGameObject(position, geom, shader, texture), origin_pos(position) {
		scale_ = glm::vec2(1.1f);
		orbit_angle = 0;
		health = GUNNER_INIT_HP;
		damage = GUNNER_INIT_DMG;
		bullet_damage = GUNNER_INIT_BULLET_DMG;
		shoot_timer.Start(GUNNER_SHOOT_CD);
	}


	/*** Update function, performs orbit movement with a moving origin ***/
	void GunnerEnemy::Update(double delta_time) {
		EnemyGameObject::Update(delta_time);
		float distance_to_player = glm::distance(target_pos, position_);

		// move the orbit origin *towards* the player if the gunner is far away
		if (distance_to_player > GUNNER_STAY_DIST) {
			origin_pos += velocity_ * GUNNER_SPEED * static_cast<float>(delta_time);
		}
		// move the orbit origin *away* from the player if the player approaches
		else if (distance_to_player < GUNNER_EVADE_DIST) {
			origin_pos -= velocity_ * GUNNER_SPEED * static_cast<float>(delta_time) * 1.5f;
		}
		
		// rotate around a the origin position (via parametric equations)
		orbit_angle += delta_time * ORBIT_SPEED;
		position_.x = origin_pos.x + ORBIT_RADIUS * cos(orbit_angle);
		position_.y = origin_pos.y + ORBIT_RADIUS * sin(orbit_angle);
	}


	/*** Specialized timer finished check, shoots and resets timer when done ***/
	bool GunnerEnemy::IsFinished() {
		if (shoot_timer.Finished()) {
			shoot_timer.Start(GUNNER_SHOOT_CD);
			return true;
		}
		else {
			return false;
		}
	}



	/***************************/
	/* ChaserEnemy Definitions */
	/***************************/

	/*** Constructor, initializes Chaser-specific defaults ***/
	ChaserEnemy::ChaserEnemy(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: EnemyGameObject(position, geom, shader, texture) {
		scale_ = glm::vec2(0.9f);
		health = CHASER_INIT_HP;
		damage = CHASER_INIT_DMG;
	}

	
	/*** Update, moves the chaser using the pursuit method ***/
	void ChaserEnemy::Update(double delta_time) {
		EnemyGameObject::Update(delta_time);
		position_.x += velocity_.x * CHASER_SPEED * delta_time;
		position_.y += velocity_.y * CHASER_SPEED * delta_time;
	}



	/*****************************/
	/* KamikazeEnemy Definitions */
	/*****************************/

	/*** Constructor, initializes Kamikaze-specific defaults ***/
	KamikazeEnemy::KamikazeEnemy(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: EnemyGameObject(position, geom, shader, texture) {
		scale_ = glm::vec2(0.6f);
		health = KAMIKAZE_INIT_HP;
		damage = KAMIKAZE_INIT_DMG;
	}

	
	/*** Update, moves the kamikaze using the pursuit method ***/
	void KamikazeEnemy::Update(double delta_time) {
		EnemyGameObject::Update(delta_time);
		position_.x += velocity_.x * KAMIKAZE_SPEED * delta_time;
		position_.y += velocity_.y * KAMIKAZE_SPEED * delta_time;
	}


	/*** Updates the target position predictively by using param velocity (given no lock-on) ***/
	void KamikazeEnemy::UpdateTarget(GameObject* obj) {
		target_pos = obj->GetPosition();
		float dist_to_player = glm::distance(target_pos, position_);

		// if the kamikaze is far enough from the player, use predictive pursuit
		if (dist_to_player > KAMIKAZE_LOCKON_DIST) {
			glm::vec3 predicted_pos = target_pos + obj->GetVelocity() * 1.5f;

			// check for overshoot before using the prediction (overshoots returns negative dot prods)
			if (glm::dot(predicted_pos - position_, target_pos - position_) >= 0) {
				target_pos = predicted_pos;
			}
		}

		velocity_ = glm::normalize(target_pos - position_);
	}

}
