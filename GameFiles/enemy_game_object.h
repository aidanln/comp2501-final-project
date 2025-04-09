// EnemyGameObject class declarations, base class for all the enemies in the game

#ifndef ENEMY_GAME_OBJECT_H_
#define ENEMY_GAME_OBJECT_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/compatibility.hpp>
#include <cmath>

#include "game_object.h"
#include "timer.h"
#include "defs.h"

namespace game {

    // Inherits from GameObject
    class EnemyGameObject : public GameObject {

    public:
        EnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

        // Base Enemy Update function, defined more in-depth in derived classes
        virtual void Update(double delta_time) override;

        // Retrieve the players updated position
        virtual void UpdateTarget(GameObject* obj);

        // Getters
        inline int GetHealth(void) const { return health; }
        inline int GetDamage(void) const { return damage; }
        inline int GetPointReward(void) const { return point_reward; }
        inline bool IsExploded(void) const { return exploded; }

        // Setters
        inline void SetDamage(int dmg) { damage = dmg; }
        inline void Explode(void) { exploded = true; }

        // Health/Damage helpers
        void TakeDamage(int recieved_dmg);

        // Scaling for health and speed
        inline void SetHealthScale(float scale_factor) { health = floor(health * scale_factor); }
        inline void SetSpeedScale(float scale_factor) { speed_scale_factor = scale_factor; }

    protected:
        // member vars to be inherited by all enemy types
        int health;
        int damage;
        unsigned short int point_reward;
        bool exploded;
        float target_angle;
        glm::vec3 target_pos;
        float speed_scale_factor;
        

    }; // class EnemyGameObject

} // namespace game

#endif // ENEMY_GAME_OBJECT_H_
