// Enemy Spawn class declarations, defines the areas which enemies spawn during waves

#ifndef ENEMY_SPAWN_H
#define ENEMY_SPAWN_H

#define GLM_FORCE_RADIANS

#include "game_object.h"

namespace game {

    class EnemySpawn : public GameObject {

        public:
            // Constructor
            EnemySpawn(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

            // Update Override to implement constant spinning
            void Update(double delta_time) override;

        private:
            // hello vro
            
    };

}

#endif // GAME_OBJECT_H_
