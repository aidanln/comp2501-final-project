// ParticleSystem Class Declarations, Inherits GameObject

#ifndef PARTICLE_SYSTEM_H_
#define PARTICLE_SYSTEM_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class ParticleSystem : public GameObject {

        public:
            ParticleSystem(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GameObject* parent);

            void Render(const glm::mat4& view_matrix, double current_time) override;

        private:
            GameObject *parent_;

    };

}

#endif // PARTICLE_SYSTEM_H_
