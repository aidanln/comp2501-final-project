// Weapon class declarations, base class for all the enemies in the game

#ifndef WEAPON_H_
#define WEAPON_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/compatibility.hpp>
#include <cmath>

#include "game_object.h"
#include "timer.h"
#include "defs.h"

namespace game {

    // Inherits from GameObject
    class Weapon {

    public:
        // Constructor
        Weapon(int damage, float firing_cooldown, float bullet_lifespan, float bullet_spread, float bullet_speed, bool semi_auto);

        // Member Getters
        inline int GetDamage(void) const { return damage; }
        inline float GetBulletSpeed() const { return bullet_speed; }
        inline float GetBulletLifespan() const { return bullet_lifespan; }
        inline float GetBulletSpread() const { return bullet_spread; }
        inline float GetFiringCooldown() const { return firing_cooldown; }
        inline bool IsSemiAuto() const { return semi_auto; }

        // Member Setters
        inline void SetDamage(int dmg) { damage = dmg; }
        inline void SetFiringCooldown(float cd) { firing_cooldown = cd; }
        inline void SetSemiAuto(bool tf) { semi_auto = tf; }

    private:
        // member vars to be inherited by all enemy types
        int damage;
        float firing_cooldown, bullet_lifespan, bullet_spread, bullet_speed;
        bool semi_auto;

    }; // class Weapon

} // namespace game

#endif // WEAPON_H_
