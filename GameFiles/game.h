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
#include "derived_enemy_objects.h"
#include "collectible_game_object.h"
#include "projectile_game_object.h"
#include "weapons.h"

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

        // Update the cursor tracker variable to be accurate each tick
        void UpdateCursorPosition(void);

        // Handle user input
        void HandleControls(double delta_time);

        // Master Update function, updates all the game objects
        void Update(double delta_time);
        
        // Enemy-Specific update helpers
        void EnemyCollisionCheck(EnemyGameObject* enemy);
        void ExplodeEnemy(EnemyGameObject* enemy);

        // Ray-Circle collision detection, checks against projectile arrays
        void EnemyShotCheck(EnemyGameObject* enemy, double delta_time);
        void PlayerShotCheck(double delta_time);

        // Collectible-Specific helpers
        void CollectItem(CollectibleGameObject* collectible);
  
        // Spawning of game objects
        void SpawnEnemy(void);
        void SpawnCollectible(void);
        void SpawnPlayerBullet(void);
        void SpawnGunnerBullet(GunnerEnemy* gunner);

        // Helper methods
        bool CollisionCheck(GameObject* obj_1, GameObject* obj_2);
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

        // Helper method for fps, needed for performance monitoring
        void DisplayFPS(double delta_time) const;


            /* PRIVATE MEMBER VARIABLES */

        // Main window: pointer to the GLFW window structure
        GLFWwindow* window_;

        // Sprite geometry
        Geometry* sprite_;
        Geometry* tiling_sprite_;

        // Shader for rendering sprites in the scene
        Shader sprite_shader_;

        // Game Object Storage, now seperated to optimize the Update() function
        PlayerGameObject* player;
        std::vector<EnemyGameObject*> enemy_arr;
        std::vector<ProjectileGameObject*> projectile_arr;
        std::vector<ProjectileGameObject*> gunner_projectile_arr;
        std::vector<CollectibleGameObject*> collectible_arr;
        GameObject* background;

        // References to textures, this needs to be a pointer
        GLuint* tex_;

        // Keep track of time
        double current_time_;

        // Timers
        Timer intro_timer;
        Timer enemy_spawn_timer;
        Timer collectible_timer;
        Timer firing_cooldown;
        Timer close_window_timer;

        // Audio Variables, should be callable 
        audio_manager::AudioManager am;
        int bg_music, game_start_sfx, boom_sfx, game_over_sfx, collect_sfx;

        // Camera Attributes, needed for smooth movement
        glm::vec3 camera_pos;
        glm::vec3 camera_target_pos;

        // Flags
        bool update_flag;
        bool holding_shoot;

        // Trackers
        glm::vec3 cursor_pos;

        // Weapons
        Weapon* pistol;
        Weapon* smg;
        Weapon* rifle;
        Weapon* sniper;

    }; // class Game

} // namespace game

#endif // GAME_H_