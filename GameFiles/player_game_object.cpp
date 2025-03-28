// PlayerGameObject class definitions

#include "player_game_object.h"

namespace game {

	/*** Constructor ***/
	PlayerGameObject::PlayerGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: GameObject(position, geom, shader, texture) {
		// initialize default values
		player_health = STARTING_HP;
		collectible_count = 0;
		angle_ = glm::pi<float>() / 2.0f;
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
	}


	/*** Decrease health by 1 (provided it's above 0) ***/
	void PlayerGameObject::DecrementHealth() {
		if (player_health > 0) {
			player_health--;
		}
	}

} // namespace game
