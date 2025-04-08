// HUD class declarations, contains vectors of objects with relevant info to display to the player 

#ifndef HUD_H_
#define HUD_H_

#define GLM_FORCE_RADIANS

#include <vector>
#include <string>
#include <sstream>
#include "text_game_object.h"

namespace game {

    // Helper class for the power-up icons
    class PowerUpIcon {

    public:
        // constructor
        PowerUpIcon(GameObject* icon, bool isActive) : icon(icon), is_active(is_active) {}
        ~PowerUpIcon() { delete icon; }

        // member vars
        GameObject* icon;
        bool is_active;

    };


    // HUD Class
    class HUD {

    public:
        // Constructor + Destructor
        HUD(Geometry* geom, Shader* text_shader, Shader* sprite_shader, GLuint font, GLuint ico1, GLuint ico2, GLuint ico3);
        ~HUD();

        // Render everything in the text_areas vector
        void RenderAll(const glm::mat4& view_matrix, double current_time);

        // Text alignment helpers, also clamps text to a certain length
        std::string LeftAlignString(const std::string& input, int len);
        std::string CenterAlignString(const std::string& input, int len);
        std::string RightAlignString(const std::string& input, int len);

        // Helper function for ensuring that the time number has less precision (less decimal places)
        std::string FormatTime(float time, int precision = 2);

        // Ensure text/icon positions are accurate
        void SetTopLeftCorner(const glm::vec3& pos);
        void SetTopRightCorner(const glm::vec3& pos);
        void SetBottomLeftCorner(const glm::vec3& pos);
        void SetBottomRightCorner(const glm::vec3& pos);
        void SetMiddleBottom(const glm::vec3& pos);

        // Update Helpers for text parts of the HUD
        void UpdatePoints(const std::string& points);
        void UpdateEnemyCount(const std::string& enemy_count);
        void UpdateWave(const std::string& wave);
        void UpdatePowerUps(bool dp_tf, bool bb_tf, bool cs_tf);
        void UpdateHealth(const std::string& health);
        void UpdateFPS(const std::string& fps);
        void UpdateTime(const double& time);
        void UpdateInfo(const std::string& info);

        // Update Helpers for icon parts of the HUD
        inline void ToggleDoublePoints(bool tf) { icon_areas[0]->is_active = tf; }
        inline void ToggleBulletBoost(bool tf) { icon_areas[1]->is_active = tf; }
        inline void ToggleColdShock(bool tf) { icon_areas[2]->is_active = tf; }

        // hide flag handling
        inline bool GetHide(void) const { return hide; }
        inline void SetHide(bool tf) { hide = tf; }
            
    private:

        // store all the text objects
        std::vector<TextGameObject*> text_areas;
            
        // store icons (for power ups)
        std::vector<PowerUpIcon*> icon_areas;

        // padding between text and icons
        glm::vec3 text_offset;
        glm::vec3 icon_offset;
        glm::vec3 icon_adj;

        // flags
        bool hide;
    };

}

#endif
