// ProjectileGameObject class definitions

#include "projectile_game_object.h"

namespace game {


	/*** Constructor ***/
	ProjectileGameObject::ProjectileGameObject(
		const glm::vec3 &position, Geometry* geom, Shader* shader, const GLuint& texture, float lifespan, int damage)
		: GameObject(position, geom, shader, texture), origin(position), bullet_lifespan(lifespan), damage(damage) {

		// default declarations
		scale_ = glm::vec2(0.45f);
		time_elapsed = 0.0f;
		impact_flag = false;

		// start the erase timer, as projectiles should auto-delete
		erase_timer_.Start(bullet_lifespan);
	}


	/*** Move the projectile based on the velocity member variable ***/
	void ProjectileGameObject::Update(double delta_time) {
		SetPosition(position_ + velocity_ * static_cast<float>(delta_time));
		time_elapsed += delta_time;
	}

} // namespace game
