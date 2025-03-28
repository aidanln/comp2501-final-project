// ProjectileGameObject class definitions

#include "projectile_game_object.h"

namespace game {


	/*** Constructor ***/
	ProjectileGameObject::ProjectileGameObject(const glm::vec3 &position, Geometry* geom, Shader* shader, const GLuint& texture)
		: GameObject(position, geom, shader, texture), origin(position) {
		// default declarations
		scale_ = glm::vec2(0.4f);
		time_elapsed = 0.0f;
		impact_flag = false;
	}


	/*** Move the projectile based on the velocity member variable ***/
	void ProjectileGameObject::Update(double delta_time) {
		SetPosition(position_ + velocity_ * static_cast<float>(delta_time));
		time_elapsed += delta_time;
	}


	/*** Setup projectile to be erased ***/
	void ProjectileGameObject::StartEraseTimer(void) {
		erase_timer_.Start(BULLET_LIFESPAN);
	}


	/*** If the timer is finished, decrement the projectileCount as the projectile will soon be deleted ***/
	bool ProjectileGameObject::EraseTimerCheck(void) {
		return erase_timer_.Finished();
	}

} // namespace game
