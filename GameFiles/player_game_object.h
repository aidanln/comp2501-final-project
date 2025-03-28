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

    private:
        // Keep track of player health
        unsigned short int player_health;

        // Keep track of invincibility mode progress
        unsigned short int collectible_count;

    }; // class PlayerGameObject

} // namespace game

#endif // PLAYER_GAME_OBJECT_H_
