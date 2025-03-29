// all derived enemy object class declarations, includes Gunner, Chaser, and Kamikaze

#ifndef DERIVED_ENEMY_OBJECTS_H_
#define DERIVED_ENEMY_OBJECTS_H_

#include "enemy_game_object.h"

namespace game {

    // orbits around a point that moves slowly towards the player, shoots at the player
    class Gunner : public EnemyGameObject {

    }; // class Gunner

    // moves quickly towards the player to hit them and cause damage
    class Chaser : public EnemyGameObject {

    }; // class Chaser

    // moves very quickly, uses predictive pursuit movement, explodes on impact
    class Kamikaze : public EnemyGameObject {

    }; // class Kamikaze

} // namespace game

#endif // ENEMY_GAME_OBJECT_H_
