// ProjectileGameObject class declarations, handles any bullets shot by the player

#ifndef PROJECTILE_GAME_OBJECT_H_
#define PROJECTILE_GAME_OBJECT_H_

#include <iostream>
#include "game_object.h"
#include "particle_system.h"
#include "defs.h"

namespace game {

    // Inherits from GameObject
    class ProjectileGameObject : public GameObject {

    public:
        // constructor
        ProjectileGameObject(const glm::vec3& position, Geometry* geom, Shader* shader,
                             const GLuint& texture, float lifespan, int damage);

        // overriden Update method, moves based on velocity vector
        void Update(double delta_time) override;

        // getters
        inline const glm::vec3& GetOrigin(void) const { return origin;  }
        inline float GetBulletTimeElapsed(void) const { return time_elapsed; }
        inline float GetDamage(void) const { return damage; }
        inline bool GetImpact(void) const { return impact_flag; }

        // setter, indicates an impact has occured
        void ImpactOccured(void);

    private:
        // private member variables
        glm::vec3 origin;
        float bullet_lifespan;
        float time_elapsed;
        int damage;
        bool impact_flag;

    }; // class ProjectileGameObject

} // namespace game

#endif // PROJECTILE_GAME_OBJECT_H_
