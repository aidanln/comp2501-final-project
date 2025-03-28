// ProjectileGameObject class declarations, handles any bullets shot by the player

#ifndef PROJECTILE_GAME_OBJECT_H_
#define PROJECTILE_GAME_OBJECT_H_

#include <iostream>
#include "game_object.h"
#include "defs.h"

namespace game {

    // Inherits from GameObject
    class ProjectileGameObject : public GameObject {

    public:
        // constructor
        ProjectileGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

        // overriden functions
        void Update(double delta_time) override; // moves based on velocity vector
        void StartEraseTimer(void) override;
        bool EraseTimerCheck(void) override; // decrements projectileCount if finished

        // getters
        inline const glm::vec3& GetOrigin(void) const { return origin;  }
        inline float GetLifespan(void) const { return time_elapsed; }
        inline bool GetImpact(void) const { return impact_flag; }

        // setter, indicates an impact has occured
        inline void ImpactOccured(void) { impact_flag = true; }

    private:
        // private member variables
        glm::vec3 origin;
        float time_elapsed;
        bool impact_flag;

    }; // class ProjectileGameObject

} // namespace game

#endif // PROJECTILE_GAME_OBJECT_H_
