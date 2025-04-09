// TextGameObject class declarations, facilitates text rendering via strings

#ifndef TEXT_GAME_OBJECT_H_
#define TEXT_GAME_OBJECT_H_

#define GLM_FORCE_RADIANS

#include <string>
#include <glm/gtc/matrix_transform.hpp>

#include "game_object.h"
#include "defs.h"

namespace game {

    // Inherits from GameObject
    class TextGameObject : public GameObject {

        public:
            TextGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture);

            // Text Getter and Setter
            inline std::string GetText(void) const { return text_; }
            inline void SetText(std::string text) { text_ = text; }

            // Render function for the text
            void Render(const glm::mat4& view_matrix, double current_time) override;

        private:
            std::string text_;

    };

}

#endif // TEXT_GAME_OBJECT_H_
