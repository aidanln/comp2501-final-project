#ifndef PARTICLE_SYSTEM_H_
#define PARTICLE_SYSTEM_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class ParticleSystem : public GameObject {

        public:
            ParticleSystem(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GameObject* parent);

            void Update(double delta_time) override;

            void Render(const glm::mat4& view_matrix, double current_time) override;

            void SetParticleScale(float x, float y) { scale_x = x; scale_y = y; }

        private:
            GameObject *parent_;
            float scale_x, scale_y;

    }; // class ParticleSystem

} // namespace game

#endif // PARTICLE_SYSTEM_H_
