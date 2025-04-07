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
        void TakeDamage(int recieved_dmg);

        // Weapon handlers
        inline Weapon* GetWeapon(void) { return weapon; }
        inline void SetWeapon(Weapon* w) { weapon = w; }
        int GetDamage(void);

        // Point handlers
        void AddPoints(int amount);
        inline int GetPoints(void) const { return points; }
        inline void SpendPoints(int amount) { points -= amount; }

        // Power-up state handlers
        void EnableDoublePoints(void);
        inline bool IsDoublePointsActive(void) const { return double_points; }
        void EnableBulletBoost(void);
        inline bool IsBulletBoostActive(void) const { return bullet_boost; }
        void EnableColdShock(void);
        inline bool IsColdShockActive(void) const { return cold_shock; }

    private:
        // Health trackers
        short int health;
        short int max_health;
        
        // Timers for handling player health
        Timer i_frames_timer;
        Timer regen_cd;
        Timer regen_step;

        // Intended angle, GameObject::angle_ uses linear interpolation to reach this
        float target_angle;

        // i need more bullets
        Weapon* weapon;

        // Track points, used for buying items and whatnot
        int points;

        // Power-up flags
        bool double_points, bullet_boost, cold_shock;

        // Power-up specific timers
        Timer dp_timer;
        Timer bb_timer;
        Timer cs_timer;
    };

}

#endif // PLAYER_GAME_OBJECT_H_
