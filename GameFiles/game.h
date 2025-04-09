// Game class declarations, holds all the main game objects and logic

#ifndef GAME_H_
#define GAME_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>
#include <string>
#include <glm/gtc/matrix_transform.hpp> 
#include <SOIL/SOIL.h>
#include <iostream>
#include <random>
#include <path_config.h>

#include "audio_manager.h"
#include "timer.h"
#include "shader.h"
#include "sprite.h"
#include "game_object.h"
#include "player_game_object.h"
#include "enemy_game_object.h"
#include "enemy_spawn.h"
#include "derived_enemy_objects.h"
#include "collectible_game_object.h"
#include "projectile_game_object.h"
#include "weapons.h"
#include "text_game_object.h"
#include "hud.h"
#include "particles.h"
#include "particle_system.h"
#include "waves.h"
#include "buyable_item.h"

#include "defs.h"

namespace game {

    class Game {

    public:

            /* PUBLIC MEMBER FUNCTIONS */

        // Constructor and destructor
        Game(); // unused, leave it for the Init() function
        ~Game();

        // Initialize graphics libraries and main window, must be called before any other method
        void Init(void);
        void InitAudio(void); // Helper method for initializing all audio related data

        // Set up the game world (scene, game objects, etc.)
        void SetupGameWorld(void);

        // Destroy the game world by freeing memory and shutting down the audio manager
        void DestroyGameWorld(void);

        // Play an intro sequence with the title of the game appearing, allows the player to get ready
        void PlayIntro(void);

        // Run the game (keep the game active) by defining a loop which we stay until the window closes
        void MainLoop(void);

    private:

            /* PRIVATE MEMBER FUNCTIONS */

        // Handle user input
        void HandleControls(double delta_time);

        // Handle purchasing an item
        void HandlePlayerPurchase(void);

        // Update the cursor tracker variable to be accurate each tick
        void UpdateCursorPosition(void);

        // Master Update function, updates all the game objects
        void Update(double delta_time);

        // Sub-Update functions, updates all specific components of the game
        void UpdateCamera(double delta_time);
        void UpdatePlayer(double delta_time);
        void UpdateBuyables(double delta_time);
        void UpdateEnemies(double delta_time);
        void UpdatePlayerProjectiles(double delta_time);
        void UpdateGunnerProjectiles(double delta_time);
        void UpdateCollectibles(double delta_time);
        void UpdateHUD(double delta_time);
        
        // Enemy-Specific update helpers
        void EnemyCollisionCheck(EnemyGameObject* enemy);
        void ExplodeEnemy(EnemyGameObject* enemy);

        // Check against projectile arrays for ray-circle collision
        void EnemyShotCheck(EnemyGameObject* enemy, double delta_time);
        void PlayerShotCheck(double delta_time);

        // Collectible-Specific helpers
        void CollectPowerUp(CollectibleGameObject* collectible);
  
        // Spawning of game objects
        void SpawnEnemy(void);
        void SpawnCollectible(EnemyGameObject* enemy);
        void SpawnPlayerBullet(void);
        void SpawnGunnerBullet(GunnerEnemy* gunner);

        // General Use Helper methods
        bool CollisionCheck(GameObject* obj_1, GameObject* obj_2);
        bool RayCircleCheck(ProjectileGameObject* bullet, GameObject* obj, float col_dist);
        void KillPlayer(void);
        void GameOver(void);
        
        // Render the game world
        void Render(void);

        // Callback for when the window is resized
        static void ResizeCallback(GLFWwindow* window, int width, int height);

        // Load all textures in the game
        void LoadTextures(std::vector<std::string>& textures);

        // Set a specific texture
        void SetTexture(const GLuint& w, const char* fname);

        // Pre-multiply alpha channels, needed for rendering transparent parts in textures
        void PremultiplyAlpha(unsigned char* image, int pixelCount);


            /* PRIVATE MEMBER VARIABLES */

        // Main window: pointer to the GLFW window structure
        GLFWwindow* window_;

        // Window Metadata Tracking
        int window_width_;
        int window_height_;

        // Sprite geometry
        Geometry* sprite_;
        Geometry* tiling_sprite_;

        // Particle geometry
        Geometry* particles_;

        // Shader for rendering sprites in the scene
        Shader sprite_shader_;

        // Shader for rendering particles
        Shader particle_shader_;

        // Shader for rendering texts
        Shader text_shader_;

        // References to textures, this needs to be a pointer
        GLuint* tex_;

        // Keep track of time
        double current_time_;

        // Game Object Storage, now seperated to optimize the Update() function
        PlayerGameObject* player;
        GameObject* vignette;
        GameObject* background;
        HUD* hud;
        std::vector<EnemyGameObject*> enemy_arr;
        std::vector<EnemySpawn*> enemy_spawn_arr;
        std::vector<ProjectileGameObject*> projectile_arr;
        std::vector<ProjectileGameObject*> gunner_projectile_arr;
        std::vector<CollectibleGameObject*> collectible_arr;
        std::vector<ParticleSystem*> particle_arr;
        std::vector<BuyableItem*> buyable_arr;

        // Wave control object
        WaveControl waves;

        // Weapon Object Storage, entity object for PlayerGameObject
        Weapon* pistol;
        Weapon* smg;
        Weapon* rifle;
        Weapon* sniper;

        // Timers
        Timer intro_timer;
        Timer enemy_spawn_timer;
        Timer firing_cooldown;
        Timer close_window_timer;
        Timer update_fps_timer;

        // Audio Variables, should be callable 
        audio_manager::AudioManager am;
        int bg_music, game_start_sfx, boom_sfx, game_over_sfx, collect_sfx, power_up_ambience,
            player_hit_sfx, player_shoot_sfx, enemy_hit_sfx, enemy_shoot_sfx, wave_complete_sfx, smg_shoot_sfx,rifle_shoot_sfx, sniper_shoot_sfx;

        // Camera Attributes, needed for smooth movement
        glm::vec3 camera_pos;
        glm::vec3 camera_target_pos;

        // Flags
        bool update_flag;
        bool holding_shoot;
        bool holding_interact;

        // Trackers
        glm::vec3 cursor_pos;
        int spawn_index;
        int interact_id;

        // Random Number Generation Helper
        std::random_device rd;

    }; // class Game

} // namespace game

#endif // GAME_H_