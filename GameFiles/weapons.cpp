// Weapon class definitions

#include "weapons.h"

#define GLM_FORCE_RADIANS

namespace game {

	/*** Constructor ***/
	Weapon::Weapon(int damage, float firing_cooldown, float bullet_lifespan, float bullet_spread, float bullet_speed, bool semi_auto) 
		: damage(damage), firing_cooldown(firing_cooldown), bullet_lifespan(bullet_lifespan), bullet_spread(bullet_spread), bullet_speed(bullet_speed), semi_auto(semi_auto) {}



} // namespace game
