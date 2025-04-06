// CollectibleGameObject class declarations, handles objects that can be collected 

#ifndef COLLECTIBLE_GAME_OBJECT_H_
#define COLLECTIBLE_GAME_OBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class CollectibleGameObject : public GameObject {

    public:
        // Constructor
        CollectibleGameObject(const glm::vec3& position, Geometry* geom, Shader* shader,
                              const GLuint& texture, int pu_id);

        // Overriden update function
        void Update(double delta_time) override;

        // Collect flag helpers
        virtual void Collect(void);
        inline bool IsCollected(void) const { return collected; }

        // Power-Up ID helpers
        inline int GetPowerUpID(void) const { return power_up_id; }

    protected:
        bool collected;

        // 0 = double points    1 = bullet boost     2 = cold shock
        unsigned short int power_up_id;

    }; // class CollectibleGameObject

} // namespace game

#endif // COLLECTIBLE_GAME_OBJECT_H_
