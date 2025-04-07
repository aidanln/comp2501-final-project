// EnemySpawn class definitions, needed for proper functionality

#define GLM_FORCE_RADIANS

#include "buyable_item.h"

namespace game {

    /*** Constructor, initalizes values to params and default values ***/
    BuyableItem::BuyableItem(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture)
        : GameObject(position, geom, shader, texture) {
        // initialize defaults
        scale_ = glm::vec2(2.5f);
    }


    /*** Overriden Update function, makes the border spin (cool visuals) ***/
    void BuyableItem::Update(double delta_time) {
        angle_ += delta_time * 0.8f;
    }

}