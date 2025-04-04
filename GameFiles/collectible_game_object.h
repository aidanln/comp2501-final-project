// CollectibleGameObject class declarations, handles objects that can be collected 

#ifndef COLLECTIBLE_GAME_OBJECT_H_
#define COLLECTIBLE_GAME_OBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class CollectibleGameObject : public GameObject {

    public:
        // Constructor
        CollectibleGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

        // Overriden update function
        void Update(double delta_time) override;

        // Collect flag helpers
        virtual void Collect(void);
        inline bool IsCollected(void) const { return collected; }

    protected:
        bool collected;

    }; // class CollectibleGameObject

} // namespace game

#endif // COLLECTIBLE_GAME_OBJECT_H_
