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
		point_reward = GUNNER_POINT_REWARD;
		bullet_damage = GUNNER_INIT_BULLET_DMG;
		time_since_last_shot = 0.0f;
	}


	/*** Update function, performs orbit movement with a moving origin ***/
	void GunnerEnemy::Update(double delta_time) {
		EnemyGameObject::Update(delta_time);

		// initalizations, provides small runtime optimizations
		float distance_to_player = glm::distance(target_pos, position_);
		float dt = static_cast<float>(delta_time);

		// move the orbit origin TOWARDS the player if the gunner is far away
		if (distance_to_player > GUNNER_STAY_DIST) {
			origin_pos += velocity_ * GUNNER_SPEED * dt;
		}

		// move the orbit origin AWAY FROM the player if the player approaches
		else if (distance_to_player < GUNNER_EVADE_DIST) {
			origin_pos -= velocity_ * GUNNER_SPEED * dt * 1.5f;
		}
		
		// rotate around a the origin position (via parametric equations)
		orbit_angle += delta_time * ORBIT_SPEED;
		position_.x = origin_pos.x + ORBIT_RADIUS * cos(orbit_angle);
		position_.y = origin_pos.y + ORBIT_RADIUS * sin(orbit_angle);

		// update time_since_last_shot
		time_since_last_shot += dt;
	}


	/*** Check if the gunner should shoot, non-timer method to help with cold shock implementation ***/
	bool GunnerEnemy::IsShootCDFinished() {
		if (time_since_last_shot > GUNNER_SHOOT_CD) {
			time_since_last_shot = 0.0f;
			return true;
		}
		else {
			return false;
		}
	}

	/***************************/
	/* ArmObject Definitions */
	/***************************/

	ArmObject::ArmObject(const glm::vec3& offset, Geometry* geom, Shader* shader, const GLuint& texture)
		: EnemyGameObject(glm::vec3(0.0f), geom, shader, texture), offset_from_parent(offset) {
		scale_ = glm::vec2(0.3f);  // small arm
	}

	void ArmObject::UpdateFromParent(const glm::vec3& parent_pos, float parent_angle, float lerp_factor) {
		// Apply rotation around parent position
		local_angle = LerpAngle(local_angle, parent_angle, lerp_factor);
		float s = sin(local_angle);
		float c = cos(local_angle);

		glm::vec3 rotated_offset;
		rotated_offset.x = offset_from_parent.x * c - offset_from_parent.y * s;
		rotated_offset.y = offset_from_parent.x * s + offset_from_parent.y * c;

		position_ = parent_pos + rotated_offset;
		SetRotation(local_angle - HALF_PI);
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
		point_reward = CHASER_POINT_REWARD;
		child1 = new ArmObject(glm::vec3(0.6f, 0.0f, 0.0f), geom, shader, texture);
		child2 = new ArmObject(glm::vec3(0.3f, 0.0f, 0.0f), geom, shader, texture);
	}

	ChaserEnemy::~ChaserEnemy() {
		delete child1;
		delete child2;
	}
	
	/*** Update, moves the chaser using the pursuit method ***/
	void ChaserEnemy::Update(double delta_time) {
		EnemyGameObject::Update(delta_time);
		position_.x += velocity_.x * CHASER_SPEED * delta_time;
		position_.y += velocity_.y * CHASER_SPEED * delta_time;
		float angle = atan2(velocity_.y, velocity_.x);  // direction facing
		child1->UpdateFromParent(position_, angle, 0.05);  // child1 rotates with the body
		child2->UpdateFromParent(child1->GetPosition(), child1->GetLocalAngle(), 0.02);
	}

	void ChaserEnemy::Render(const glm::mat4& view_matrix, double current_time) {
		EnemyGameObject::Render(view_matrix, current_time);
		child1->Render(view_matrix, current_time);
		child2->Render(view_matrix, current_time);
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
		point_reward = KAMIKAZE_POINT_REWARD;
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
