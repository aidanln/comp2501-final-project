// PlayerGameObject class declarations, contains all the data and functionality for the user-controlled player

#ifndef PLAYER_GAME_OBJECT_H_
#define PLAYER_GAME_OBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class PlayerGameObject : public GameObject {

    public:
        // Constructor
        PlayerGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

        // Update function for moving the player object around
        void Update(double delta_time) override;

        // Update the target angle, allows for cursor-based rotation
        inline void UpdateTargetAngle(float ta) { target_angle = ta; }

        // Health handlers
        inline unsigned short int GetHealth(void) const { return health; }
        void TakeDamage(int recieved_dmg);
        
        // Damage handlers
        inline int GetDamage(void) const { return damage; }
        inline void SetDamage(int dmg) { damage = dmg; }

        // TEMPORARY, collectible handlers
        inline unsigned short int GetCollectibleCount(void) const { return collectible_count; }
        inline void IncrementCollectibleCount(void) { collectible_count++; }

    private:
        unsigned short int health;
        unsigned short int max_health;
        unsigned short int damage;

        // Timers for handling player health
        Timer i_frames_timer;
        Timer regen_cd;
        Timer regen_step;

        // Intended angle, GameObject::angle_ uses linear interpolation to reach this
        float target_angle;

        // TEMPORARY, amount of collectibles the player has
        unsigned short int collectible_count;

    };

}

#endif // PLAYER_GAME_OBJECT_H_
