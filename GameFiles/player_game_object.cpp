// PlayerGameObject class definitions

#include "player_game_object.h"

namespace game {

	/*** Constructor ***/
	PlayerGameObject::PlayerGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: GameObject(position, geom, shader, texture) {
		// initialize default values
		player_health = STARTING_HP;
		player_damage = 2;
		collectible_count = 0;
		angle_ = 0;
		target_angle = 0;
	}


	/*** Player-Specific Update function ***/
	void PlayerGameObject::Update(double delta_time) {
		float dt = static_cast<float>(delta_time);

		// Update velocity based on acceleration
		velocity_ += acceleration_ * dt;

		// Ensure velocity doesn't exceed max speed
		if (glm::length(velocity_) > MAX_SPEED) {
			velocity_ = glm::normalize(velocity_) * MAX_SPEED;
		}

		// Update position based on velocity, then decelerate
		position_ += velocity_ * dt;
		velocity_ /= 1.01 + (dt * 3);

		// Update rotation using lerp with the target angle, ensures smooth motion
		float difference = target_angle - angle_;
		difference = std::remainder(difference, 2.0f * glm::pi<float>());
		angle_ += difference * 0.1f;
	}


	/*** Decrease health by 1 (provided it's above 0) ***/
	void PlayerGameObject::TakeDamage(int damage) {
		if (player_health > 0) {
			player_health -= damage;
		}
		if (player_health < 0) {
			player_health = 0;
		}
		std::cout << "Lost health! Player health is now " << player_health << "." << std::endl;
	}

} // namespace game
