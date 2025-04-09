// all derived enemy object class declarations, includes Gunner, Chaser, and Kamikaze

#ifndef DERIVED_ENEMY_OBJECTS_H_
#define DERIVED_ENEMY_OBJECTS_H_

#include "enemy_game_object.h"

namespace game {

    /*
    * Gunner -> orbits around a point that moves slowly towards the player, shoots at the player
    */ 
    class GunnerEnemy : public EnemyGameObject {

    public:
        // Constructor, has unique default initializations
        GunnerEnemy(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

        // Gunner-specific movement
        void Update(double delta_time) override;

        // Check if the shooting cooldown has finished
        bool IsShootCDFinished();

        // Bullet damage helper
        inline int GetBulletDamage(void) const { return bullet_damage; }

    private:
        // movement vars
        glm::vec3 origin_pos;
        float orbit_angle;

        // shooting vars
        unsigned short int bullet_damage;
        float time_since_last_shot;
    };


    /*
    * Arm -> children of chaser, is transformed based on parent
    */
    class ArmObject : public EnemyGameObject {
    public:
        ArmObject(const glm::vec3& offset, Geometry* geom, Shader* shader, const GLuint& texture);

        void UpdateFromParent(const glm::vec3& parent_pos, float parent_angle, float lerp_factor);
        inline float GetLocalAngle() const { return local_angle; }
        void UpdateSawbladeRotation(double delta_time);
        inline float GetSawbladeRotationSpeed() const { return sawblade_rotation_speed_; }
        inline void SetSawbladeRotationSpeed(float speed) { sawblade_rotation_speed_ = speed; }
        float GetSawbladeRotation() const { return sawblade_rotation_; }
        void SetIsSawblade(bool is_saw);
        void ArmObject::Render(const glm::mat4& view_matrix, double current_time) override;

    private:
        // dist from parent
        glm::vec3 offset_from_parent;
        float local_angle;
        float sawblade_rotation_speed_;
        float sawblade_rotation_;
        bool is_sawblade;
    };


    /*
    * Chaser -> moves quickly towards the player to hit them and cause damage
    */
    class ChaserEnemy : public EnemyGameObject {

    public:
        // Constructor, has unique default initializations
        ChaserEnemy(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture, const GLuint& base_texture, const GLuint& link_texture, const GLuint& saw_texture);
        ~ChaserEnemy();

        // Chaser-specific movement
        void Update(double delta_time) override;

        void ChaserEnemy::Render(const glm::mat4& view_matrix, double current_time);

        ArmObject* GetChild1() const { return child1; }
        ArmObject* GetChild2() const { return child2; }
        ArmObject* GetChild3() const { return child3; }
        int GetChildCount() const { return child_count; }

    private:
        // children
        ArmObject* child1;
        ArmObject* child2;
        ArmObject* child3;
        int child_count;
    };


    /*
    * Kamikaze -> moves very quickly, uses predictive pursuit movement, explodes on impact
    */
    class KamikazeEnemy : public EnemyGameObject {

    public:
        // Constructor, has unique default initializations
        KamikazeEnemy(const glm::vec3& position, Geometry* geom, Shader* shader, const GLuint& texture);

        // Kamikaze-specific movement
        void Update(double delta_time) override;

        void UpdateTarget(GameObject* obj) override;

        // Methods for kamikaze explosion D.O.T effect
        bool IsExplosionActive() const { return explosion_active; }
        void ActivateExplosion() { explosion_active = true; }
        float GetExplosionRadius() const { return explosion_radius; }
        void SetExplosionRadius(float r) { explosion_radius = r; }

    private:
        // Member variables for kamikaze explsion D.O.T effect
        float explosion_radius;
        bool explosion_active;
    };

}

#endif // ENEMY_GAME_OBJECT_H_