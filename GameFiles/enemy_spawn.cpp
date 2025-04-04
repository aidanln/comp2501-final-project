// EnemySpawn class definitions, needed for proper functionality

#define GLM_FORCE_RADIANS

#include "enemy_spawn.h"

namespace game {

    /*** Constructor, initalizes values to params and default values ***/
    EnemySpawn::EnemySpawn(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture) 
        : GameObject(position, geom, shader, texture) {
        // initialize defaults
        scale_ = glm::vec2(3.5f);
    }


    /*** Overriden Update function, just makes the portal spin on its axis for cool visuals ***/
    void EnemySpawn::Update(double delta_time) {
        angle_ += delta_time * 0.5f;
    }

}