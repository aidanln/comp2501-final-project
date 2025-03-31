// all derived enemy object class declarations, includes Gunner, Chaser, and Kamikaze

#ifndef DERIVED_ENEMY_OBJECTS_H_
#define DERIVED_ENEMY_OBJECTS_H_

#include "enemy_game_object.h"

namespace game {


    /*
    * Gunner -> orbits around a point that moves slowly towards the player, shoots at the player
    */ 
    class GunnerEnemy : public EnemyGameObject {

    public:
        // Constructor, has unique default initializations
        GunnerEnemy(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

        // Gunner-specific movement
        void Update(double delta_time) override;

    private:
        glm::vec3 target_pos;
        glm::vec3 origin_pos;
        float orbit_angle;

    };


    /*
    * Chaser -> moves quickly towards the player to hit them and cause damage
    */
    class ChaserEnemy : public EnemyGameObject {

    public:
        // Constructor, has unique default initializations
        ChaserEnemy(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

        // Chaser-specific movement
        void Update(double delta_time) override;

    private:
        glm::vec3 target_pos;

    };


    /*
    * Kamikaze -> moves very quickly, uses predictive pursuit movement, explodes on impact
    */
    class KamikazeEnemy : public EnemyGameObject {

    public:
        // Constructor, has unique default initializations
        KamikazeEnemy(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

        // Kamikaze-specific movement
        void Update(double delta_time) override;

    private:
        glm::vec3 target_pos;

    };

}

#endif // ENEMY_GAME_OBJECT_H_
