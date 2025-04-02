// GameObject base class definitions

#define GLM_FORCE_RADIANS

#include "game_object.h"

namespace game {

    /*** Constructor, initalizes values to params and default values ***/
    GameObject::GameObject(const glm::vec3 &position, Geometry* geom, Shader* shader, const GLuint& texture)
        : position_(position), geometry_(geom), shader_(shader), texture_(texture) {
        // default initializations
        scale_ = glm::vec2(1.0f);
        angle_ = 0.0f;
        velocity_ = glm::vec3(0.0f);
        acceleration_ = glm::vec3(0.0f);
        ghost_ = false;
    }


    /*** Base update function, does nothing as it is overriden ***/
    void GameObject::Update(double delta_time) {}


    /*** Universal render function for all game objects ***/
    void GameObject::Render(const glm::mat4& view_matrix, double current_time) {

        // Set up the shader
        shader_->Enable();

        // Set up the view matrix
        shader_->SetUniformMat4("view_matrix", view_matrix);

        // Handle ghost mode
        shader_->SetUniform1i("ghost", ghost_);

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

        // Draw the entity
        glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);
    }


    /*** Get the direction an object is facing towards ***/
    const glm::vec3& GameObject::GetBearing(void) const {
        glm::vec3 dir(cos(angle_), sin(angle_), 0.0);
        return dir;
    }


    /*** Get the right side of an object ***/
    const glm::vec3& GameObject::GetRight(void) const {
        float pi_over_two = HALF_PI;
        glm::vec3 dir(cos(angle_ - pi_over_two), sin(angle_ - pi_over_two), 0.0);
        return dir;
    }


    /*** Rotation setter ***/
    void GameObject::SetRotation(float angle) { 
        // Set rotation angle for the object, ensure angle is in the range [0, 2*pi]
        angle = fmod(angle, TWO_PI);
        if (angle < 0.0){
            angle += TWO_PI;
        }
        angle_ = angle;
    }

    
    /*** Rotate based on linear interpolation, ensures smooth movement ***/
    float GameObject::LerpAngle(float current, float target, float lerp_factor) {

        // normalize angles to [0, 2pi] to ensure consistency
        current = glm::mod(current, TWO_PI);
        target = glm::mod(target, TWO_PI);

        // figure out if we should rotate clockwise or counter-clockwise
        float difference = target - current;
        float alternate = difference - glm::sign(difference) * TWO_PI;

        // Choose the smallest absolute difference (shortest path)
        if (glm::abs(difference) > glm::abs(alternate)) {
            difference = alternate;
        }

        // Interpolate and wrap the result
        float result = current + difference * lerp_factor;
        return glm::mod(result, TWO_PI);
    }


    /*** Checks if the erase timer has finished ***/
    bool GameObject::EraseTimerCheck() {
        return erase_timer_.Finished();
    }


    /*** Base Erase Timer initalizer, defaults to 5 seconds ***/
    void GameObject::StartEraseTimer() {
        erase_timer_.Start(5.0f);
    }

} // namespace game
