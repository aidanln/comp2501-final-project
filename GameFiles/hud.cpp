// HUD class definitions, all the in-depth functionality of the header functions

#include "hud.h"

namespace game {

    /*** Constructor, initalizes all the text areas, icon areas, and member vars ***/
    HUD::HUD(Geometry* geom, Shader* text_shader, Shader* sprite_shader, GLuint font, GLuint ico1, GLuint ico2, GLuint ico3) {

        // initialize member vars
        text_offset = glm::vec3(0.0f, 0.5f, 0.0f);
        icon_offset = glm::vec3(0.6f, 0.0f, 0.0f);
        icon_adj = glm::vec3(1.1f, 0.0f, 0.0f);
        hide = true;

        // helper pre-def, initializes everything offscreen
        glm::vec3 init_pos(-100.0f, -100.0f, 1.0f);

        /* BOTTOM LEFT HUD ELEMENTS */ 

        // ta:index 0 -> Points tracker         (top)
        TextGameObject* points = new TextGameObject(init_pos, geom, text_shader, font);
        points->SetScale(glm::vec2(CORNER_TEXT_SIZE_X, CORNER_TEXT_SIZE_Y));
        text_areas.push_back(points);
        // ta:index 1 -> Enemy Count tracker    (mid)
        TextGameObject* enemy_count = new TextGameObject(init_pos, geom, text_shader, font);
        enemy_count->SetScale(glm::vec2(CORNER_TEXT_SIZE_X, CORNER_TEXT_SIZE_Y));
        text_areas.push_back(enemy_count);
        // ta:index 2 -> Wave tracker           (bot)
        TextGameObject* wave = new TextGameObject(init_pos, geom, text_shader, font);
        wave->SetScale(glm::vec2(CORNER_TEXT_SIZE_X, CORNER_TEXT_SIZE_Y));
        text_areas.push_back(wave);

        /* BOTTOM RIGHT HUD ELEMENTS */

        // ia:index 0 -> double points tracker  (top)
        GameObject* dp_icon = new GameObject(init_pos, geom, sprite_shader, ico1);
        dp_icon->SetScale(glm::vec2(ICON_SIZE));
        icon_areas.push_back(new PowerUpIcon(dp_icon, false));
        // ia:index 1 -> bullet boost tracker   (top)
        GameObject* bb_icon = new GameObject(init_pos, geom, sprite_shader, ico2);
        bb_icon->SetScale(glm::vec2(ICON_SIZE));
        icon_areas.push_back(new PowerUpIcon(bb_icon, false));
        // ia:index 2 -> cold shock tracker     (top)
        GameObject* cs_icon = new GameObject(init_pos, geom, sprite_shader, ico3);
        cs_icon->SetScale(glm::vec2(ICON_SIZE));
        icon_areas.push_back(new PowerUpIcon(cs_icon, false));
        // ta:index 3 -> Health tracker         (bot)
        TextGameObject* health = new TextGameObject(init_pos, geom, text_shader, font);
        health->SetScale(glm::vec2(CORNER_TEXT_SIZE_X, CORNER_TEXT_SIZE_Y));
        text_areas.push_back(health);

        /* TOP RIGHT HUD ELEMENTS */

        // ta:index 4 -> FPS tracker            (top)
        TextGameObject* fps = new TextGameObject(init_pos, geom, text_shader, font);
        fps->SetScale(glm::vec2(CORNER_TEXT_SIZE_X, CORNER_TEXT_SIZE_Y));
        text_areas.push_back(fps);

        /* TOP LEFT HUD ELEMENTS */

        // ta:index 5 -> Time tracker           (top)
        TextGameObject* time = new TextGameObject(init_pos, geom, text_shader, font);
        time->SetScale(glm::vec2(CORNER_TEXT_SIZE_X, CORNER_TEXT_SIZE_Y));
        text_areas.push_back(time);

        /* BELOW THE HUD ELEMENTS */

        // ta:index 6 -> info segment 1         (top)
        TextGameObject* info1 = new TextGameObject(init_pos, geom, text_shader, font);
        info1->SetScale(glm::vec2(INFO_TEXT_SIZE_X, INFO_TEXT_SIZE_Y));
        text_areas.push_back(info1);
        // ta:index 7 -> info segment 2         (bot)
        TextGameObject* info2 = new TextGameObject(init_pos, geom, text_shader, font);
        info2->SetScale(glm::vec2(INFO_TEXT_SIZE_X, INFO_TEXT_SIZE_Y));
        text_areas.push_back(info2);
        
    }


    /*** Destructor, frees all the memory used for TextGameObjects and PowerUpIcon ***/
    HUD::~HUD() {
        for (int i = 0; i < text_areas.size(); ++i) {
            delete text_areas[i];
        }
        for (int i = 0; i < icon_areas.size(); ++i) {
            delete icon_areas[i];
        }
    }


    /*** Call the Render function for all the HUD elements ***/
    void HUD::RenderAll(const glm::mat4& view_matrix, double current_time) {

        // Skip if the hide flag is true
        if (hide) return;

        // Render the Text
        for (int i = 0; i < text_areas.size(); ++i) {
            text_areas[i]->Render(view_matrix, current_time);
        }

        // Render the Icons
        int icons_rendered = 0;
        for (int i = 0; i < icon_areas.size(); ++i) {
            if (icon_areas[i]->is_active) {
                GameObject* icon_obj = icon_areas[i]->icon;
                icon_obj->SetPosition(icon_obj->GetPosition() + ((float)icons_rendered * -icon_offset));
                icon_obj->Render(view_matrix, current_time);
                icons_rendered++;
            }
        }
    }


    /*** Return a fixed length, left-aligned string by adding spaces on the right ***/
    std::string HUD::LeftAlignString(const std::string& input, int len) {
        std::string result;

        // truncate input if it exceeds the max length
        if (input.length() >= len) {
            result = input.substr(0, len);
        }

        // add spaces to the input if it's not long enough
        else {
            result = input;
            result.append(len - input.length(), ' ');
        }

        return result;
    }


    /*** Return a fixed length, center-aligned string by adding spaces on both sides ***/
    std::string HUD::CenterAlignString(const std::string& input, int len) {
        std::string result;

        // truncate input if it exceeds the max length
        if (input.length() >= len) {
            result = input.substr(0, len);
        }
        else {
            int padding = len - input.length();
            int left_padding = padding / 2;
            int right_padding = padding - left_padding;

            result.append(left_padding, ' ');
            result.append(input);
            result.append(right_padding, ' ');
        }

        return result;
    }


    /*** Return a fixed length, right-aligned string by adding spaces on the left ***/
    std::string HUD::RightAlignString(const std::string& input, int len) {
        std::string result;

        // truncate input if it exceeds the max length
        if (input.length() >= len) {
            result = input.substr(0, len);
        }
        else {
            result.append(len - input.length(), ' ');
            result.append(input);
        }

        return result;
    }


    /*** Format the time input to have less precision, looks nicer on the HUD ***/
    std::string HUD::FormatTime(float time, int precision) {
        std::ostringstream out;
        out.precision(precision);
        out << std::fixed << time;
        return out.str();
    }


    /*** Update the coordinates for HUD elements to be drawn ***/
    void HUD::SetBottomLeftCorner(const glm::vec3& pos) {
        text_areas[0]->SetPosition(pos + (2.0f * text_offset));
        text_areas[1]->SetPosition(pos + text_offset);
        text_areas[2]->SetPosition(pos);
    }
    void HUD::SetBottomRightCorner(const glm::vec3& pos) {
        icon_areas[0]->icon->SetPosition(pos + text_offset + icon_adj);
        icon_areas[1]->icon->SetPosition(pos + text_offset + icon_adj);
        icon_areas[2]->icon->SetPosition(pos + text_offset + icon_adj);
        text_areas[3]->SetPosition(pos);
    }
    void HUD::SetTopRightCorner(const glm::vec3& pos) {
        text_areas[4]->SetPosition(pos);
    }
    void HUD::SetTopLeftCorner(const glm::vec3& pos) {
        text_areas[5]->SetPosition(pos);
    }
    void HUD::SetMiddleBottom(const glm::vec3& pos) {
        text_areas[6]->SetPosition(pos);
        text_areas[7]->SetPosition(pos - text_offset);
    }


    /*** Update the text components of the HUD elements ***/
    void HUD::UpdatePoints(const std::string& points) {
        text_areas[0]->SetText(
            LeftAlignString(("Points: " + points), SMALL_HUD_LEN)
        );
    }
    void HUD::UpdateEnemyCount(const std::string& enemy_count) {
        text_areas[1]->SetText(
            LeftAlignString(("Enemies Left: " + enemy_count), SMALL_HUD_LEN)
        );
    }
    void HUD::UpdateWave(const std::string& wave) {
        text_areas[2]->SetText(
            LeftAlignString(("Wave: " + wave), SMALL_HUD_LEN)
        );
    }
    void HUD::UpdatePowerUps(bool dp_tf, bool bb_tf, bool cs_tf) {
        ToggleDoublePoints(dp_tf);
        ToggleBulletBoost(bb_tf);
        ToggleColdShock(cs_tf);
    }
    void HUD::UpdateHealth(const std::string& health) {
        text_areas[3]->SetText(
            RightAlignString(("Player HP: " + health), SMALL_HUD_LEN)
        );
    }
    void HUD::UpdateFPS(const std::string& fps) {
        text_areas[4]->SetText(
            RightAlignString(("FPS: " + fps), SMALL_HUD_LEN)
        );

    }
    void HUD::UpdateTime(const double& time) {
        float timeValue = static_cast<float>(time);
        std::string formattedTime = FormatTime(timeValue);
        text_areas[5]->SetText(
            LeftAlignString(("Time: " + formattedTime), SMALL_HUD_LEN)
        );
    }
    void HUD::UpdateTopInfo(const std::string& info) {
        text_areas[6]->SetText(
            CenterAlignString(info, LONG_HUD_LEN)
        );
    }
    void HUD::UpdateBottomInfo(const std::string& info) {
        text_areas[7]->SetText(
            CenterAlignString(info, LONG_HUD_LEN)
        );
    }

}