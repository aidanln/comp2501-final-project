// PlayerGameObject class declarations, contains all the data and functionality for the user-controlled player

#ifndef PLAYER_GAME_OBJECT_H_
#define PLAYER_GAME_OBJECT_H_

#include "game_object.h"
#include "defs.h"

namespace game {

    // Inherits from GameObject
    class PlayerGameObject : public GameObject {

    public:
        // Constructor
        PlayerGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

        // Update function for moving the player object around
        void Update(double delta_time) override;

        // getters
        inline unsigned short int GetHealth(void) const { return player_health; }
        inline unsigned short int GetCollectibleCount(void) const { return collectible_count; }

        // Methods for handling health and collectible trackers
        inline void IncrementCollectibleCount(void) { collectible_count++; }
        void DecrementHealth(void);

        // Methods for handling cursor-based rotation
        inline void UpdateTargetAngle(float ta) { target_angle = ta; }

    private:
        // HP counter
        unsigned short int player_health;

        // Tracks invincibility mode progress
        unsigned short int collectible_count;

        // The angle the player should be looking at
        float target_angle;

    }; // class PlayerGameObject

} // namespace game

#endif // PLAYER_GAME_OBJECT_H_
