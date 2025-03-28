// CollectibleGameObject class definitions

#include "collectible_game_object.h"

namespace game {

	/*** Constructor ***/
	CollectibleGameObject::CollectibleGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: GameObject(position, geom, shader, texture) {
		// default values
		scale_ = glm::vec2(0.8f);
		collected = false;
	}


	/*** Update function for Collectible Objects ***/
	void CollectibleGameObject::Update(double delta_time) {
		// nothing right now
	}


	/*** Handle collecting the object ***/
	void CollectibleGameObject::Collect(void) {
		collected = true;
		ghost_ = true;
		StartEraseTimer();
	}

} // namespace game
