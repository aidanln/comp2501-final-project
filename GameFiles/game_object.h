// GameObject base class declarations
// responsible for handling the rendering and updating of an object in the game world

#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#define GLEW_STATIC

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "geometry.h"
#include "timer.h"
#include "defs.h"

namespace game {

    class GameObject {

        public:
            // Constructor
            GameObject(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

            // Deconstructor + Polymorphism Helper
            virtual ~GameObject() = default;

            // Update the GameObject's state. Can be overriden in children
            virtual void Update(double delta_time);

            // Renders the GameObject 
            virtual void Render(const glm::mat4& view_matrix, double current_time);

            // Getters
            inline const glm::vec3& GetPosition(void) const { return position_; }
            inline const glm::vec2& GetScale(void) const { return scale_; }
            inline float GetXRadius(void) const { return scale_.x / 2; }
            inline float GetYRadius(void) const { return scale_.y / 2; }
            inline const glm::vec3& GetVelocity(void) const { return velocity_; }
            inline const glm::vec3& GetAcceleration(void) const { return acceleration_; }
            inline bool IsGhost() const { return ghost_; }
            inline float GetRotation(void) const { return angle_; }
            glm::vec3 GetBearing(void) const;
            glm::vec3 GetRight(void) const;

            // Setters
            inline void SetPosition(const glm::vec3& position) { position_ = position; }
            inline void SetScale(const glm::vec2& scale) { scale_ = scale; }
            inline void SetTexture(const GLuint& texture) { texture_ = texture; }
            inline void SetVelocity(const glm::vec3& velocity) { velocity_ = velocity; }
            inline void SetAcceleration(const glm::vec3& acceleration) { acceleration_ = acceleration; }
            inline void SetGhost(bool ghost) { ghost_ = ghost; }
            void SetRotation(float angle);

            // Linear Interpolation helper
            float LerpAngle(float current, float target, float lerp_factor);

            // Timer handling
            virtual void StartEraseTimer(void); 
            virtual bool EraseTimerCheck(void);

            // Object Hiding (scale=0 and a flag) Handling
            void Hide(void);
            inline bool IsHidden(void) const { return hidden_; }
            

        protected:
            // Object's Transform Variables
            glm::vec3 position_;
            glm::vec2 scale_;
            float angle_;
            glm::vec3 velocity_;
            glm::vec3 acceleration_;
            bool ghost_;
            bool hidden_;

            // Geometry
            Geometry *geometry_;
 
            // Shader
            Shader *shader_;

            // Object's texture reference
            GLuint texture_;

            // object timer for explosion
            Timer erase_timer_;
            
    }; // class GameObject

} // namespace game

#endif // GAME_OBJECT_H_
