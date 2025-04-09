// TextGameObject class definitions, facilitates text rendering via strings

#include "text_game_object.h"

namespace game {

    /*** Constructor, uses GameObject constructor and initializes default values ***/
    TextGameObject::TextGameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture)
        : GameObject(position, geom, shader, texture) {
        text_ = "";
        scale_ = glm::vec2(7.2f, 0.9f);
    }


    /*** Text-Specific Render function ***/
    void TextGameObject::Render(const glm::mat4& view_matrix, double current_time) {

        // Set up the shader
        shader_->Enable();

        // Set up the view matrix
        shader_->SetUniformMat4("view_matrix", view_matrix);

        // Setup the scaling matrix for the shader
        glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale_.x, scale_.y, 1.0));

        // Setup the rotation matrix for the shader
        glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), angle_, glm::vec3(0.0, 0.0, 1.0));

        // Set up the translation matrix for the shader
        glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position_);

        // Setup the transformation matrix for the shader
        glm::mat4 transformation_matrix = translation_matrix * rotation_matrix * scaling_matrix;

        // Set the transformation matrix in the shader
        shader_->SetUniformMat4("transformation_matrix", transformation_matrix);

        // Set up the geometry
        geometry_->SetGeometry(shader_->GetShaderProgram());

        // Bind the entity's texture
        glBindTexture(GL_TEXTURE_2D, texture_);

        // Ensure the text doesnt exceed 40 chars
        constexpr auto MAX_TEXT_LENGTH = 40;

        // Set text length
        int final_size = text_.size();
        if (final_size > MAX_TEXT_LENGTH){
            final_size = MAX_TEXT_LENGTH;
        }

        shader_->SetUniform1i("text_len", final_size);

        // Set the text data
        GLint data[MAX_TEXT_LENGTH]{};
        for (int i = 0; i < final_size; i++){
            data[i] = text_[i];
        }
        shader_->SetUniformIntArray("text_content", final_size, data);

        // Draw the entity
        glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);
    }

} // namespace game
