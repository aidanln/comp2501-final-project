// Definitions for each of the PowerUp classes

#include "power_ups.h"

namespace game {

	DoublePoints::DoublePoints(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: CollectibleGameObject(position, geom, shader, texture) {}

	BulletBoost::BulletBoost(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: CollectibleGameObject(position, geom, shader, texture) {}

	ColdShock::ColdShock(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
		: CollectibleGameObject(position, geom, shader, texture) {}

} // namespace game
