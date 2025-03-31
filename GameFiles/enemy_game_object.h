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

        // Base Enemy Update function, defined more in-depth in derived classes
        virtual void Update(double delta_time) override;

        // Retrieve the players updated position
        virtual void UpdateTarget(GameObject* obj);

        // Exploded State Handlers
        inline void Explode(void) { exploded = true; }
        inline bool IsExploded(void) const { return exploded; }
        inline int GetDamage() const { return enemy_damage;  }

    protected:
        glm::vec3 target_pos;
        bool exploded;

        int enemy_health;

        int enemy_damage;

    }; // class EnemyGameObject

} // namespace game

#endif // ENEMY_GAME_OBJECT_H_
