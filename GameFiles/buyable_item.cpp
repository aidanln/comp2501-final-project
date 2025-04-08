// BuyableItem class definitions

#define GLM_FORCE_RADIANS

#include "buyable_item.h"

namespace game {

    /*** Constructor, initalizes values to params, sets defaults, initializes the icon ***/
    BuyableItem::BuyableItem(const glm::vec3& position, Geometry* geom, Shader* shader,
        const GLuint& ring_tex, const GLuint& icon_tex)
        : GameObject(position, geom, shader, ring_tex) {

        // initialize defaults
        point_cost = 100;
        name_and_cost = "";
        scale_ = glm::vec2(1.8f);

        // create the icon
        icon = new GameObject(position, geom, shader, icon_tex);
    }

    
    /*** Destructor, just frees space for the icon ***/
    BuyableItem::~BuyableItem() {
        delete icon;
    }


    /*** Overriden Update function, makes the border spin (cool visuals) ***/
    void BuyableItem::Update(double delta_time) {
        angle_ += delta_time * 0.6f;
    }

}