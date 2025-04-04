// CollectibleGameObject class definitions

#include "collectible_game_object.h"

namespace game {

	/*** Constructor ***/
	CollectibleGameObject::CollectibleGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: GameObject(position, geom, shader, texture) {

		// initalize default values
		scale_ = glm::vec2(0.8f);
		collected = false;

		// start erase_timer, as collectibles should auto delete after a period of time
		erase_timer_.Start(COLLECTIBLE_DURATION);
	}


	/*** Update function for Collectible Objects ***/
	void CollectibleGameObject::Update(double delta_time) {
		position_ += velocity_ * COLLECTIBLE_SPEED * static_cast<float>(delta_time);
	}


	/*** Handle collecting the object ***/
	void CollectibleGameObject::Collect(void) {
		collected = true;
		ghost_ = true;
	}

} // namespace game
