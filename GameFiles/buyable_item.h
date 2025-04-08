// BuyableItem class declarations, defines the areas where you can buy weapons and upgrades

#ifndef BUYABLE_ITEM_H
#define BUYABLE_ITEM_H

#include "game_object.h"

namespace game {

    class BuyableItem : public GameObject {

        public:
            // Constructor
            BuyableItem(const glm::vec3& position, Geometry* geom, Shader* shader,
                const GLuint& ring_tex, const GLuint& icon_tex);
            ~BuyableItem();

            // Update Override to implement constant spinning
            void Update(double delta_time) override;

            // Return the icon object for rendering
            inline GameObject* GetIcon(void) const { return icon; }

            // Point Cost helpers
            inline void SetPointCost(int cost) { point_cost = cost; }
            inline int GetPointCost(void) const { return point_cost; }

            // String Info helpers
            inline void SetNameAndCost(const std::string& str) { name_and_cost = str; }
            inline const std::string& GetNameAndCost(void) const { return name_and_cost; }

        private:
            GameObject* icon;
            unsigned short int point_cost;

            // display name_and_cost on the info segment of the HUD
            std::string name_and_cost;

    };

}

#endif
