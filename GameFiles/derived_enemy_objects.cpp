// Definitions for every derived enemy object

#include "derived_enemy_objects.h"
#include <vector>

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
			origin_pos += velocity_ * GUNNER_SPEED * speed_scale_factor * dt;
		}

		// move the orbit origin AWAY FROM the player if the player approaches
		else if (distance_to_player < GUNNER_EVADE_DIST) {
			origin_pos -= velocity_ * GUNNER_SPEED * speed_scale_factor * dt * 1.5f;
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
	/* ArmObject Definitions   */
	/***************************/

	/*** Constructor, default values, ititalize off-screen ***/
	ArmObject::ArmObject(const glm::vec3& offset, Geometry* geom, Shader* shader, const GLuint& texture)
		: EnemyGameObject(glm::vec3(100.0f, 100.0f, 1.0f), geom, shader, texture), offset_from_parent(offset) {
		scale_ = glm::vec2(0.3f);
		local_angle = 0.0f;
		sawblade_rotation_speed_ = 0.0f;
		sawblade_rotation_ = 0.0f;
		is_sawblade = false;
	}


	/*** Update position and rotation based on parent properties ***/
	void ArmObject::UpdateFromParent(const glm::vec3& parent_pos, float parent_angle, float lerp_factor) {

		// Apply rotation around parent position
		local_angle = LerpAngle(local_angle, parent_angle, lerp_factor);
		float s = sin(local_angle);
		float c = cos(local_angle);

		// Calculate rotated offset
		glm::vec3 rotated_offset;
		rotated_offset.x = offset_from_parent.x * c - offset_from_parent.y * s;
		rotated_offset.y = offset_from_parent.x * s + offset_from_parent.y * c;

		// apply offsets
		position_ = parent_pos + rotated_offset;
		SetRotation(local_angle - HALF_PI);
	}

	void ArmObject::UpdateSawbladeRotation(double delta_time) {
		if (is_sawblade) {
			sawblade_rotation_ += sawblade_rotation_speed_ * static_cast<float>(delta_time);
			// Keep the angle within 0 to 2*PI to prevent potential floating-point issues
			if (sawblade_rotation_ > 2 * glm::pi<float>()) {
				sawblade_rotation_ -= 2 * glm::pi<float>();
			}
		}
	}

	/*** Set whether this arm is a sawblade ***/
	void ArmObject::SetIsSawblade(bool is_saw) {
		is_sawblade = is_saw;
		if (is_sawblade) {
			sawblade_rotation_speed_ = 2.5f; // Faster rotation for sawblade
		}
	}

	/*** Override render to allow rotation ***/
	void ArmObject::Render(const glm::mat4& view_matrix, double current_time) {
		if (is_sawblade) {
			// Save current rotation
			float original_rotation = GetRotation();

			// Add sawblade rotation
			SetRotation(sawblade_rotation_);

			// Render with combined rotation
			GameObject::Render(view_matrix, current_time);

			// Restore original rotation
			SetRotation(original_rotation);
		}
		else {
			GameObject::Render(view_matrix, current_time);
		}
	}



	/***************************/
	/* ChaserEnemy Definitions */
	/***************************/

	/*** Constructor, initializes Chaser-specific defaults ***/
	ChaserEnemy::ChaserEnemy(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture, const GLuint& base_texture, const GLuint& link_texture, const GLuint& saw_texture)
		: EnemyGameObject(position, geom, shader, texture) {
		scale_ = glm::vec2(0.9f);
		health = CHASER_INIT_HP;
		damage = CHASER_INIT_DMG;
		point_reward = CHASER_POINT_REWARD;
		child_count = 3;
		child1 = new ArmObject(glm::vec3(0.55f, 0.0f, 0.0f), geom, shader, base_texture);
		child2 = new ArmObject(glm::vec3(0.2f, 0.0f, 0.0f), geom, shader, link_texture);
		child3 = new ArmObject(glm::vec3(0.4f, 0.0f, 0.0f), geom, shader, saw_texture);
		child1->SetScale(glm::vec2(0.6f));
		child2->SetScale(glm::vec2(0.7f, 0.55f));
		child3->SetScale(glm::vec2(0.7f));
	}


	/*** Delete children from hierarchical chain link ***/
	ChaserEnemy::~ChaserEnemy() {
		delete child1;
		delete child2;
		delete child3;
	}
	

	/*** Update, moves the chaser using the pursuit method, updates children based on parent ***/
	void ChaserEnemy::Update(double delta_time) {
		EnemyGameObject::Update(delta_time);
		position_.x += velocity_.x * CHASER_SPEED * speed_scale_factor * delta_time;
		position_.y += velocity_.y * CHASER_SPEED * speed_scale_factor * delta_time;

		// calculate direction facing
		float angle = atan2(velocity_.y, velocity_.x);

		// child1 transforms based on body
		child1->UpdateFromParent(position_, angle, 0.03);

		// child2 transforms based on child1
		child2->UpdateFromParent(child1->GetPosition(), child1->GetLocalAngle(), 0.01);

		// child3 transforms based on child2
		child3->UpdateFromParent(child2->GetPosition(), child2->GetLocalAngle(), 0.01);

		child3->SetIsSawblade(true);
		child3->UpdateSawbladeRotation(delta_time);

		// std::cout << child1->GetPosition().x << ", " << child1->GetPosition().y << std::endl;
	}


	/*** Override render function to render parent and children ***/
	void ChaserEnemy::Render(const glm::mat4& view_matrix, double current_time) {
		
		child2->Render(view_matrix, current_time);

		child3->Render(view_matrix, current_time);

		child1->Render(view_matrix, current_time);
		
		EnemyGameObject::Render(view_matrix, current_time);
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
		explosion_radius = 0.0f;
		explosion_active = false;
	}

	
	/*** Update, moves the kamikaze using the pursuit method ***/
	void KamikazeEnemy::Update(double delta_time) {
		EnemyGameObject::Update(delta_time);
		position_.x += velocity_.x * KAMIKAZE_SPEED * speed_scale_factor * delta_time;
		position_.y += velocity_.y * KAMIKAZE_SPEED * speed_scale_factor * delta_time;
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
