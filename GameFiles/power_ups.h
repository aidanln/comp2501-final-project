// Every Power Up class declaration and functionality, gives the player a buff over a certain time interval 

#ifndef POWER_UPS_H_
#define POWER_UPS_H_

#include "collectible_game_object.h"

namespace game {

    /*
    * Double Points Power-Up -> Doubles points recieved by the player when defeating enemies
    */ 
    class DoublePoints : public CollectibleGameObject {

    public:
        // Constructor
        DoublePoints(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

    private:
        // nothing rn

    };


    /*
    * Double Points Power-Up -> Triples player damage
    */
    class BulletBoost : public CollectibleGameObject {

    public:
        // Constructor
        BulletBoost(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

    private:
        // nothing rn

    };


    /*
    * Cold Shock Power-Up -> halves the speed of all the enemies
    */
    class ColdShock : public CollectibleGameObject {

    public:
        // Constructor
        ColdShock(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

    private:
        // nothing rn

    };

}

#endif // COLLECTIBLE_GAME_OBJECT_H_
