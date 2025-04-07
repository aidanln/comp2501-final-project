// Enemy Spawn class declarations, defines the areas which enemies spawn during waves

#ifndef BUYABLE_ITEM_H
#define BUYABLE_ITEM_H

#include "game_object.h"

namespace game {

    class BuyableItem : public GameObject {

        public:
            // Constructor
            BuyableItem(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

            // Update Override to implement constant spinning
            void Update(double delta_time) override;

        private:
            // hello vro
            
    };

}

#endif
