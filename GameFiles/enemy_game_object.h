// EnemyGameObject class declarations, base class for all the enemies in the game

#ifndef ENEMY_GAME_OBJECT_H_
#define ENEMY_GAME_OBJECT_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <cmath>
#include "game_object.h"
#include "timer.h"
#include "defs.h"

namespace game {

    // Inherits from GameObject
    class EnemyGameObject : public GameObject {

    public:
        EnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

        // Update function for moving the enemy object around
        void Update(double delta_time) override;

        // Retrieve the players updated position
        void UpdateTarget(const glm::vec3& position);
        bool TargetUpdateCheck(void);

        // Exploded State Handlers
        inline void Explode(void) { exploded = true; }
        inline bool IsExploded(void) const { return exploded; }

        // Movement State Handlers
        inline bool GetState(void) const { return state; }
        inline void SetState(bool new_state) { state = new_state; }

    private:
        
        // Flags
        bool exploded; // True means the enemy is dead, false means alive
        bool state; // 0 = patrolling, 1 = intercepting, represented as a bool to save memory

        // Patrol movement handling
        glm::vec3 origin_pos;
        float orbit_angle;

        // Chase movement handling
        glm::vec3 target_pos;
        Timer target_update_timer;

    }; // class EnemyGameObject

} // namespace game

#endif // ENEMY_GAME_OBJECT_H_
