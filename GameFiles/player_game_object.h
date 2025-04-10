// PlayerGameObject class declarations, contains all the data and functionality for the user-controlled player

#ifndef PLAYER_GAME_OBJECT_H_
#define PLAYER_GAME_OBJECT_H_

#include "game_object.h"
#include "weapons.h"

namespace game {

    // Inherits from GameObject
    class PlayerGameObject : public GameObject {

    public:
        // Constructor
        PlayerGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

        // Overriden Update function for moving the player object around
        void Update(double delta_time) override;

        // Overriden EraseTimer handler, takes longer than the default
        void StartEraseTimer(void) override;

        // Update the target angle, allows for cursor-based rotation
        inline void UpdateTargetAngle(float ta) { target_angle = ta; }
        
        // Health handlers
        inline unsigned short int GetHealth(void) const { return health; }
        bool TakeDamage(int recieved_dmg);

        // Speed handlers
        inline float GetAccelForce(void) const { return accel_force; }

        // Weapon handlers
        inline Weapon* GetWeapon(void) { return weapon; }
        inline void SetWeapon(Weapon* w) { weapon = w; }
        int GetDamage(void);

        // Weapon ID handlers -> 0=pistol, 1=smg, 2=rifle, 3=sniper
        inline int GetWeaponID(void) const { return weapon_id; }
        inline void SetWeaponID(int id) { weapon_id = id; }

        // Point handlers
        void AddPoints(int amount);
        inline int GetPoints(void) const { return points; }
        inline void SpendPoints(int amount) { points -= amount; }

        // Knockback stuff
        inline Timer GetKnockbackCooldown(void) const { return knockback_cooldown; }
        void ApplyKnockback(glm::vec3& direction, int damage);

        // Power-up state handlers
        void EnableDoublePoints(void);
        inline bool IsDoublePointsActive(void) const { return double_points; }
        void EnableBulletBoost(void);
        inline bool IsBulletBoostActive(void) const { return bullet_boost; }
        void EnableColdShock(void);
        inline bool IsColdShockActive(void) const { return cold_shock; }

        // Upgrade state handlers
        void EnableArmorPlating(void);
        inline bool IsArmorPlatingActive(void) const { return armor_plating; }

        void EnableRegenCoating(void);
        inline bool IsRegenCoatingActive(void) const { return regen_coating; }

        void EnableNitroInfuse(void);
        inline bool IsNitroInfuseActive(void) const { return nitro_infuse; }

        void EnableCelestialAugment(void);
        inline void DisableCelestialAugment(void) { celestial_augment = false; }
        inline bool IsCelestialAugmentActive(void) const { return celestial_augment; }

    private:
        // Health member vars
        short int health;
        short int max_health;
        float regen_cd_time;
        float regen_step_amount;
        
        // max speed
        float max_speed;
        float accel_force;

        // Timers for handling player health
        Timer i_frames_timer;
        Timer regen_cd;
        Timer regen_step;
        Timer knockback_cooldown;

        // Intended angle, GameObject::angle_ uses linear interpolation to reach this
        float target_angle;

        // i need more bullets
        Weapon* weapon;
        unsigned short int weapon_id;

        // Track points, used for buying items and whatnot
        int points;

        // Power-up helpers (limited, on timer)
        bool double_points, bullet_boost, cold_shock;
        Timer dp_timer;
        Timer bb_timer;
        Timer cs_timer;

        // Upgrade helpers (permanent, buyable)
        bool armor_plating, regen_coating, nitro_infuse, celestial_augment;

    };

}

#endif // PLAYER_GAME_OBJECT_H_
