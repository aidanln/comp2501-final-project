// Game class definitions, defines all the main methods for the game

#define GLM_FORCE_RADIANS
#define GLEW_STATIC

#include "game.h"

// Directory with game resources such as textures
const std::string resources_directory_g = RESOURCES_DIRECTORY;

// Constants for the OpenGL window and viewport
const char* window_title_g = "Celestial Onslaught";
const unsigned int window_width_g = 1200;
const unsigned int window_height_g = 800;
const glm::vec3 viewport_background_color_g(0.1, 0.1, 0.1);

namespace game {

    /*** Constructor is unused, replaced by Init() ***/
    Game::Game(void) {}


    /*** Destructor ***/
    Game::~Game(void) {

        // Free memory used by game world
        DestroyGameWorld(); 

        // Free rendering resources
        delete sprite_;
        delete tiling_sprite_;

        // Close window
        glfwDestroyWindow(window_);
        glfwTerminate();
    }


    /*** Initialize Game and GLFW/GLEW data ***/
    void Game::Init(void) {

        // Initialize the window management library (GLFW)
        if (!glfwInit()) {
            throw(std::runtime_error(std::string("Could not initialize the GLFW library")));
        }

        // Set whether window can be resized
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

        // Create a window and its OpenGL context
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g, NULL, NULL);
        if (!window_) {
            glfwTerminate();
            throw(std::runtime_error(std::string("Could not create window")));
        }

        // Make the window's OpenGL context the current one
        glfwMakeContextCurrent(window_);

        // Initialize the GLEW library to access OpenGL extensions
        // Need to do it after initializing an OpenGL context
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            throw(std::runtime_error(std::string("Could not initialize the GLEW library: ") + std::string((const char*)glewGetErrorString(err))));
        }

        // Set event callbacks
        glfwSetFramebufferSizeCallback(window_, ResizeCallback);

        // Setup the window icon
        std::string icon_path = resources_directory_g + "/textures/enemy_spawn.png";
        int icon_width, icon_height;
        unsigned char* icon_pixels = SOIL_load_image(icon_path.c_str(), &icon_width, &icon_height, 0, SOIL_LOAD_RGBA);

        // Set the window icon
        if (icon_pixels) {
            GLFWimage icon{};
            icon.width = icon_width;
            icon.height = icon_height;
            icon.pixels = icon_pixels;
            glfwSetWindowIcon(window_, 1, &icon);
            SOIL_free_image_data(icon_pixels); // Free the memory used
        }
        else {
            std::cerr << "Failed to load window icon: " << icon_path << std::endl;
        }

        // Initialize sprite geometry
        sprite_ = new Sprite();
        sprite_->CreateGeometry();
        tiling_sprite_ = new Sprite();
        tiling_sprite_->CreateTilingGeometry();

        // Initialize sprite shader
        sprite_shader_.Init((resources_directory_g + std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/sprite_fragment_shader.glsl")).c_str());

        // Initialize time
        current_time_ = 0.0;

        // Start all the Timers
        enemy_spawn_timer.Start(ENEMY_SPAWN_DELAY);
        collectible_timer.Start(COLLECTIBLE_SPAWN_DELAY);
        firing_cooldown.Start(PLAYER_SHOOT_CD);

        // Initialize default member variables
        update_flag = false;
        holding_shoot = false;
        camera_pos = glm::vec3(0.0f);
        camera_target_pos = glm::vec3(0.0f);
        cursor_pos = glm::vec3(0.0f);

        // Audio Setup
        InitAudio();
    }


    /*** Initialize audio files and data ***/
    void Game::InitAudio(void) {
        try {
            // Initialize audio manager and position of listener
            am.Init(NULL);
            am.SetListenerPosition(0.0, 0.0, 0.0);

            // Setup the background music (bg_music)
            std::string filename = std::string(resources_directory_g).append("/audio/bg_music.wav");
            bg_music = am.AddSound(filename.c_str());
            am.SetSoundPosition(bg_music, 0.0, 0.0, 0.0); // sound properties
            am.SetLoop(bg_music, true); // loop until program ends

            // Setup the game starting sound
            filename = std::string(resources_directory_g).append("/audio/game_start.wav");
            game_start_sfx = am.AddSound(filename.c_str());
            am.SetSoundPosition(game_start_sfx, 0.0, 0.0, 0.0);

            // Setup the explosion sound
            filename = std::string(resources_directory_g).append("/audio/explosion.wav");
            boom_sfx = am.AddSound(filename.c_str());
            am.SetSoundPosition(boom_sfx, 0.0, 0.0, 0.0);

            // Setup the game over sound
            filename = std::string(resources_directory_g).append("/audio/game_over.wav");
            game_over_sfx = am.AddSound(filename.c_str());
            am.SetSoundPosition(game_over_sfx, 0.0, 0.0, 0.0);

            // Setup the collect sound
            filename = std::string(resources_directory_g).append("/audio/collect.wav");
            collect_sfx = am.AddSound(filename.c_str());
            am.SetSoundPosition(collect_sfx, 0.0, 0.0, 0.0);
        }

        // Error handling
        catch (std::exception& e) {
            PrintException(e);
        }
    }


    /*** Setup the game world ***/
    void Game::SetupGameWorld(void) {

        // Declare and load textures
        std::vector<std::string> textures;
        enum {
            tex_player = 0,
            tex_gunner = 1,
            tex_chaser = 2,
            tex_kamikaze = 3,
            tex_stars = 4,
            tex_explosion = 5,
            tex_collectible = 6,
            tex_bullet = 7,
            tex_portal = 8
        };
        textures.push_back("/textures/player_ship.png");    // 0, tex_player
        textures.push_back("/textures/gunner_ship.png");    // 1, tex_gunner
        textures.push_back("/textures/chaser_ship.png");    // 2, tex_chaser
        textures.push_back("/textures/kamikaze_ship.png");  // 3, tex_kamikaze
        textures.push_back("/textures/background.png");     // 4, tex_stats
        textures.push_back("/textures/explosion.png");      // 5, tex_explosion
        textures.push_back("/textures/collectible.png");    // 6, tex_collectible
        textures.push_back("/textures/bullet.png");         // 7, tex_bullet
        textures.push_back("/textures/enemy_spawn.png");    // 8, tex_portal
        LoadTextures(textures);

        // Setup the player object (position, texture, vertex count)
        player = new PlayerGameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[tex_player]);

        // Setup starter enemies
        enemy_arr.push_back(new GunnerEnemy(glm::vec3(-4.0f, -2.0f, 0.0f), sprite_, &sprite_shader_, tex_[tex_gunner]));
        enemy_arr.push_back(new ChaserEnemy(glm::vec3(4.0f, -2.0f, 0.0f), sprite_, &sprite_shader_, tex_[tex_chaser]));
        enemy_arr.push_back(new KamikazeEnemy(glm::vec3(0.0f, -4.5f, 0.0f), sprite_, &sprite_shader_, tex_[tex_kamikaze]));

        // Setup starter collectibles
        collectible_arr.push_back(new CollectibleGameObject(glm::vec3(-2.0f, 2.0f, 0.0f), sprite_, &sprite_shader_, tex_[tex_collectible]));
        collectible_arr.push_back(new CollectibleGameObject(glm::vec3(0.0f, 3.0f, 0.0f), sprite_, &sprite_shader_, tex_[tex_collectible]));
        collectible_arr.push_back(new CollectibleGameObject(glm::vec3(2.0f, 2.0f, 0.0f), sprite_, &sprite_shader_, tex_[tex_collectible]));

        // Setup background
        background = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), tiling_sprite_, &sprite_shader_, tex_[tex_stars]);
        background->SetScale(glm::vec2(30.0f));

        // Weapons
        pistol = new Weapon(PISTOL_INIT_DMG, PLAYER_SHOOT_CD, PLAYER_BULLET_LIFESPAN, PLAYER_BULLET_SPREAD, PLAYER_BULLET_SPEED, PLAYER_SEMI_AUTO);
        smg = new Weapon(4, 0.08f, 0.2f, 0.25f, 20.0f, false);
        rifle = new Weapon(10, 0.125f, 0.5f, 0.2f, 25.0f, false);
        sniper = new Weapon(30, 0.8f, 1.5f, 0.0f, 40.0f, true);
        player->SetWeapon(pistol);
    }


    /*** Destroy the game world, pseudo destructor ***/
    void Game::DestroyGameWorld(void) {

        // delete player
        delete player;

        // delete enemies
        for (int i = 0; i < enemy_arr.size(); ++i) {
            delete enemy_arr[i];
        }

        // delete lingering player projectiles
        for (int i = 0; i < projectile_arr.size(); ++i) {
            delete projectile_arr[i];
        }

        // delete lingering gunner projectiles
        for (int i = 0; i < gunner_projectile_arr.size(); ++i) {
            delete gunner_projectile_arr[i];
        }

        // delete collectibles
        for (int i = 0; i < collectible_arr.size(); ++i) {
            delete collectible_arr[i];
        }

        // shut down audio manager
        am.ShutDown();
    }


    /*** Play the Intro sequence, might be better to put this in MainLoop() idk ***/
    void Game::PlayIntro(void) {

        // initialize the intro
        intro_timer.Start(INTRO_DURATION);
        am.PlaySound(game_start_sfx);
        double last_time = glfwGetTime();

        // hang until the intro timer is done
        while (!intro_timer.Finished()) {
            double current_time = glfwGetTime();
            double delta_time = current_time - last_time;
            last_time = current_time;

            // from MainLoop()
            glfwPollEvents();
            HandleControls(delta_time);
            Render();
            glfwSwapBuffers(window_);
            if (FPS_CAP != 0) {
                while (1 / delta_time > FPS_CAP) {
                    delta_time = glfwGetTime() - last_time;
                }
            }
        }

        // intro is done, so allow gameplay updates, play bg music, then run MainLoop (in main.cpp)
        update_flag = true;
        am.PlaySound(bg_music);
    }


    /*** Loops during gameplay ***/
    void Game::MainLoop(void) {

        // Loop while the user did not close the window
        double last_time = glfwGetTime();
        while (!glfwWindowShouldClose(window_)) {

            // Check for game over
            if (close_window_timer.Finished()) { GameOver(); }

            // Calculate delta time
            double current_time = glfwGetTime();
            double delta_time = current_time - last_time;
            last_time = current_time;

            // Update window events like input handling
            glfwPollEvents();

            // Update the cursor position
            UpdateCursorPosition();

            // Handle user input
            HandleControls(delta_time);

            // Update all the game objects
            Update(delta_time);

            // Check if timers are finished, unless a game over occurs
            if (update_flag && RANDOM_SPAWNING) {
                if (enemy_spawn_timer.Finished()) {
                    SpawnEnemy();
                    enemy_spawn_timer.Start(ENEMY_SPAWN_DELAY);
                }
                if (collectible_timer.Finished()) {
                    SpawnCollectible();
                    collectible_timer.Start(COLLECTIBLE_SPAWN_DELAY);
                }
            }

            // Render all the game objects
            Render();

            // Push buffer drawn in the background onto the display
            glfwSwapBuffers(window_);

            // Enforce FPS cap
            if (FPS_CAP != 0) {
                while (1 / delta_time > FPS_CAP) {
                    delta_time = glfwGetTime() - last_time;
                }
            }
        }
    }


    /*** Get the coordinates of the mouse cursor in the gameworld ***/
    void Game::UpdateCursorPosition(void) {

        // initialize cursor position (relative to monitor) and window size
        double mouse_x, mouse_y;
        glfwGetCursorPos(window_, &mouse_x, &mouse_y);
        int window_width, window_height;
        glfwGetWindowSize(window_, &window_width, &window_height);

        // bounds check, abort if the mouse is outside the window
        if (mouse_x < 0 || mouse_x > window_width || mouse_y < 0 || mouse_y > window_height) {
            return;
        }

        // cast width/height to float and initialize cursor position (relative to game world)
        float width = static_cast<float>(window_width);
        float height = static_cast<float>(window_height);
        glm::vec3 temp_pos(0.0f);

        // case 1: horizontal aspect ratio, case 2: vertical aspect ratio
        if (width >= height) {
            float aspect_ratio = width / height;
            temp_pos.x = ((2.0f * mouse_x - width) * aspect_ratio) / (width * CAMERA_ZOOM);
            temp_pos.y = (-2.0f * mouse_y + height) / (height * CAMERA_ZOOM);
        }
        else {
            float aspect_ratio = height / width;
            temp_pos.x = (2.0f * mouse_x - width) / (width * CAMERA_ZOOM);
            temp_pos.y = ((-2.0f * mouse_y + height) * aspect_ratio) / (height * CAMERA_ZOOM);
        }

        // abort if the location is too close to the player
        if (glm::length(temp_pos) < 0.02f) {
            return;
        }

        // update the tracker variable
        cursor_pos = temp_pos;
    }


    /*** Handle player inputs ***/
    void Game::HandleControls(double delta_time) {

        // Handle exiting the game (keys: ESC)
        if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == 1) {
            glfwSetWindowShouldClose(window_, true);
        }

        // Print FPS, temporary solution until the HUD is created
        if (glfwGetKey(window_, GLFW_KEY_F) == 1) {
            DisplayFPS(delta_time);
        }

        // Only handle input if necessary, i.e. game is still running
        if (update_flag) {

            // define local vars to help with motion
            glm::vec3 up(0.0f, 1.0f, 0.0f);
            glm::vec3 left(-1.0f, 0.0f, 0.0f);
            glm::vec3 accel(0.0f);

            // Handle physics-based movement input (keys: W, A, S, D)
            if (glfwGetKey(window_, GLFW_KEY_W) == 1) {
                accel += up * PLAYER_ACCEL_FORCE;
            }
            if (glfwGetKey(window_, GLFW_KEY_S) == 1) {
                accel -= up * PLAYER_ACCEL_FORCE;
            }
            if (glfwGetKey(window_, GLFW_KEY_A) == 1) {
                accel += left * PLAYER_ACCEL_FORCE;
            }
            if (glfwGetKey(window_, GLFW_KEY_D) == 1) {
                accel -= left * PLAYER_ACCEL_FORCE;
            }

            // Handle firing a bullet (mouse: LEFT-CLICK)
            if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == 1) {
                if (firing_cooldown.Finished() && !holding_shoot) {
                    SpawnPlayerBullet();
                    firing_cooldown.Start(player->GetWeapon()->GetFiringCooldown());

                    // prevent semi-auto weapons from being shot by holding
                    if (player->GetWeapon()->IsSemiAuto()) {
                        holding_shoot = true;
                    }
                }
            }
            // reset holding_shoot back to false if left-click isn't pressed
            else { 
                holding_shoot = false;
            }

            // Update player acceleration based on input
            player->SetAcceleration(accel);

            // Debug, kills the player instantly (keys: K)
            if (glfwGetKey(window_, GLFW_KEY_K) == 1) {
                KillPlayer();
            }
        }
    }


    /*** Update all the game objects ***/
    void Game::Update(double delta_time) {

        // Update the Camera Position
        camera_target_pos = player->GetPosition();
        camera_pos = glm::mix(camera_pos, camera_target_pos, CAMERA_SMOOTHNESS); // ensures smooth movement

        // Update the Player
        if (update_flag) {
            if (player->GetHealth() <= 0) {
                KillPlayer();
            }
            else {
                player->UpdateTargetAngle(atan2(cursor_pos.y, cursor_pos.x) - (HALF_PI));
                player->Update(delta_time);
                PlayerShotCheck(delta_time);
            }
        }

        if (player->EraseTimerCheck()) {
            player->Hide();
        }
        
        // Update the Enemies
        for (int i = 0; i < enemy_arr.size(); ++i) {
            EnemyGameObject* enemy = enemy_arr[i];

            if (enemy->EraseTimerCheck()) {
                enemy_arr.erase(enemy_arr.begin() + i);
                delete enemy;
            }

            else if (!enemy->IsExploded()) {

                if (enemy->GetHealth() <= 0) {
                    ExplodeEnemy(enemy);
                }
                else {
                    enemy->Update(delta_time);

                    if (update_flag) {
                        if (GunnerEnemy* gunner = dynamic_cast<GunnerEnemy*>(enemy)) {
                            if (gunner->IsFinished()) {
                                SpawnGunnerBullet(gunner);
                            }
                        }
                        enemy->UpdateTarget(player);
                        EnemyShotCheck(enemy, delta_time);
                        EnemyCollisionCheck(enemy);
                    }
                }
            }
        }

        // Update Projectiles shot by the player
        for (int i = 0; i < projectile_arr.size(); ++i) {
            ProjectileGameObject* bullet = projectile_arr[i];

            bullet->Update(delta_time);

            if (bullet->EraseTimerCheck()) {
                projectile_arr.erase(projectile_arr.begin() + i);
                delete bullet;
            }
        }

        // Update Projectiles shot by the gunner enemies
        for (int i = 0; i < gunner_projectile_arr.size(); ++i) {
            ProjectileGameObject* bullet = gunner_projectile_arr[i];

            bullet->Update(delta_time);

            if (bullet->EraseTimerCheck()) {
                gunner_projectile_arr.erase(gunner_projectile_arr.begin() + i);
                delete bullet;
            }
        }
        
        // Update the Collectibles
        for (int i = 0; i < collectible_arr.size(); ++i) {
            CollectibleGameObject* collectible = collectible_arr[i];

            if (collectible->EraseTimerCheck()) {
                collectible_arr.erase(collectible_arr.begin() + i);
                delete collectible;
            }
            else if (CollisionCheck(player, collectible) && !collectible->IsCollected()) {
                CollectItem(collectible);
            }
        }
    }


    /*** Check for collisions with the param enemy ***/
    void Game::EnemyCollisionCheck(EnemyGameObject* enemy) {
        if (CollisionCheck(player, enemy)) {
            enemy->TakeDamage(enemy->GetHealth());
            player->TakeDamage(enemy->GetDamage());
        }
    }


    /*** Explode an enemy game object ***/
    void Game::ExplodeEnemy(EnemyGameObject* enemy) {

        // replace destroyer sprite with an explosion, prepare for deletion via erase timer
        enemy->Explode();
        enemy->SetTexture(tex_[5]); // explosion texture
        enemy->SetScale(glm::vec2(1.8f));
        enemy->StartEraseTimer();

        // play explosion sound
        am.PlaySound(boom_sfx);
    }


    /*** Check for ray-circle collision between an enemy and player-shot bullets ***/
    void Game::EnemyShotCheck(EnemyGameObject* enemy, double delta_time) {

        // check against all non-impacted bullets
        for (int i = 0; i < projectile_arr.size(); i++) {
            if (!projectile_arr[i]->GetImpact()) {
                ProjectileGameObject* bullet = projectile_arr[i];
                float collision_dist = enemy->GetXRadius() + BULLET_RADIUS;

                // perform ray-circle check via quadratic formula re-arrange
                glm::vec3 origin_to_center = bullet->GetOrigin() - enemy->GetPosition();
                float a = glm::dot(bullet->GetVelocity(), bullet->GetVelocity());
                float b = 2.0f * glm::dot(origin_to_center, bullet->GetVelocity());
                float c = glm::dot(origin_to_center, origin_to_center) - collision_dist * collision_dist;

                // if discriminant is greater/equal zero, we have a collision
                float discriminant = b * b - 4 * a * c;
                if (discriminant >= 0) {

                    // compute intersection points and check with the bullet's lifespan to confirm collision
                    float t1 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
                    float t2 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
                    float lifespan = bullet->GetLifespan();
                    if (t1 <= lifespan && lifespan <= t2) {

                        // collision confirmed, handle it
                        bullet->ImpactOccured();
                        bullet->Hide();
                        enemy->TakeDamage(bullet->GetDamage());
                    }
                }
            }
        }
    }


    /*** Check for ray-circle collision between the player and enemy-shot bullets ***/
    void Game::PlayerShotCheck(double delta_time) {
        float collision_dist = player->GetXRadius() + BULLET_RADIUS;

        // check against all non-impacted bullets
        for (int i = 0; i < gunner_projectile_arr.size(); i++) {
            if (!gunner_projectile_arr[i]->GetImpact()) {
                ProjectileGameObject* bullet = gunner_projectile_arr[i];

                // perform ray-circle check via quadratic formula re-arrange
                glm::vec3 origin_to_center = bullet->GetOrigin() - player->GetPosition();
                float a = glm::dot(bullet->GetVelocity(), bullet->GetVelocity());
                float b = 2.0f * glm::dot(origin_to_center, bullet->GetVelocity());
                float c = glm::dot(origin_to_center, origin_to_center) - collision_dist * collision_dist;

                // if discriminant is greater/equal zero, we have a collision
                float discriminant = b * b - 4 * a * c;
                if (discriminant >= 0) {

                    // compute intersection points and check with the bullet's lifespan to confirm collision
                    float t1 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
                    float t2 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
                    float lifespan = bullet->GetLifespan();
                    if (t1 <= lifespan && lifespan <= t2) {

                        // collision confirmed, handle it
                        bullet->ImpactOccured();
                        bullet->Hide();
                        player->TakeDamage(bullet->GetDamage());
                    }
                }
            }
        }
    }


    /*** Handle collecting a collectible ***/
    void Game::CollectItem(CollectibleGameObject* collectible) {
        player->IncrementCollectibleCount();
        collectible->Collect();
        am.PlaySound(collect_sfx);
    }


    /*** Spawn a destroyer in a random position on screen, triggered via timer (6 seconds) ***/
    void Game::SpawnEnemy(void) {
        // TEMPORARY, WILL BE REFACTORED ONCE ENEMY SPAWNS ARE ADDED

        // Random Number Generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis_x(-6.0f, 6.0f);
        std::uniform_real_distribution<> dis_y(-4.0f, 4.0f);

        // generate the x and y coordinates, must be a certain distance from the player
        glm::vec3 rand_pos(0.0f);
        while (glm::length(rand_pos) < 3.0f) {
            rand_pos.x = dis_x(gen);
            rand_pos.y = dis_y(gen);
        }

        // add a new enemy at the random position (ALWAYS GUNNER RN, CHANGE LATER)
        enemy_arr.push_back(new GunnerEnemy(rand_pos, sprite_, &sprite_shader_, tex_[1]));
    }


    /*** Spawn a collectible in a random position on screen, triggered via timer(8 seconds) ***/
    void Game::SpawnCollectible(void) {
        // TEMPORARY, WILL BE REMOVED ONCE POWER-UPS ARE ADDED

        // Random Number Generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis_x(-7.0f, 7.0f);
        std::uniform_real_distribution<> dis_y(-4.6f, 4.6f);

        // generate the x and y coordinates, must be a certain distance from the player
        glm::vec3 rand_pos(0.0f);
        while (glm::length(rand_pos) < 3.0f) {
            rand_pos.x = dis_x(gen);
            rand_pos.y = dis_y(gen);
        }

        // add a new collectible at the random position
        collectible_arr.push_back(new CollectibleGameObject(rand_pos, sprite_, &sprite_shader_, tex_[6]));
    }


    /*** Spawn a bullet at the player's position with a velocity/rotation corresponding to the player's ***/
    void Game::SpawnPlayerBullet(void) {

        // create the bullet object and add to collection
        ProjectileGameObject* bullet = new ProjectileGameObject(player->GetPosition(), sprite_, &sprite_shader_,
                                                                tex_[7], player->GetWeapon()->GetBulletLifespan(), player->GetWeapon()->GetDamage());
        projectile_arr.push_back(bullet);
        bullet->StartEraseTimer();

        // random gen for bullet spread
        std::random_device rd;
        std::mt19937 gen(rd());
        float spread = player->GetWeapon()->GetBulletSpread();
        std::uniform_real_distribution<> dis_spread(-spread, spread);

        float random_spread;
        random_spread = dis_spread(gen);

        // Calculate direction
        glm::vec3 direction = glm::normalize(cursor_pos);
        float base_angle = atan2(direction.y, direction.x);
        float spread_angle = base_angle + random_spread;
        glm::vec3 spread_direction = glm::vec3(cos(spread_angle), sin(spread_angle), 0);

        // Apply velocity with spread
        bullet->SetVelocity(spread_direction * player->GetWeapon()->GetBulletSpeed());

        // Apply rotation with spread
        bullet->SetRotation(spread_angle - HALF_PI);
    }


    /*** Spawn a bullet at the gunner's position with a velocity/rotation corresponding to the gunner's ***/
    void Game::SpawnGunnerBullet(GunnerEnemy* gunner) {

        // create the bullet object and add to collection
        ProjectileGameObject* bullet = new ProjectileGameObject(gunner->GetPosition(), sprite_, &sprite_shader_,
                                                                tex_[7], GUNNER_BULLET_LIFESPAN, gunner->GetBulletDamage());
        gunner_projectile_arr.push_back(bullet);
        bullet->StartEraseTimer();

        // set appropriate physical properties
        glm::vec3 aim_line = player->GetPosition() - gunner->GetPosition();
        bullet->SetVelocity(glm::normalize(aim_line) * GUNNER_BULLET_SPEED);
        bullet->SetRotation(atan2(aim_line.y, aim_line.x) - (HALF_PI));
    }


    /*** Handle Circle-Circle collision checking ***/
    bool Game::CollisionCheck(GameObject* obj_1, GameObject* obj_2) {
        return glm::length(obj_1->GetPosition() - obj_2->GetPosition())
               < obj_1->GetXRadius() + obj_2->GetXRadius();
        /*
        * note: this only considers the x-axis of the scale to calculate radius,
        * which is okay for now, but will become a problem with non-uniform scaling.
        */ 
    }


    /*** Handle the player explosion once health hits 0 ***/
    void Game::KillPlayer() {

        // Player Specific
        player->SetTexture(tex_[5]); // explosion texture
        player->SetRotation(0);
        player->SetScale(glm::vec2(2.5f));
        player->StartEraseTimer();
        am.PlaySound(boom_sfx);

        // Game Specific
        close_window_timer.Start(6.0f);
        update_flag = false;
        am.PlaySound(game_over_sfx);
    }


    /*** Trigger a Game Over ***/
    void Game::GameOver(void) {
        std::cout << "\n\tGAME OVER!\n" << std::endl;
        glfwSetWindowShouldClose(window_, true);
    }


    /*** Render the Game World ***/
    void Game::Render(void) {

        // Clear background
        glClearColor(viewport_background_color_g.r,
                     viewport_background_color_g.g,
                     viewport_background_color_g.b, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use aspect ratio to properly scale the window
        int width, height;
        glfwGetWindowSize(window_, &width, &height);
        glm::mat4 window_scale_matrix;
        if (width > height) {
            float aspect_ratio = ((float) width)/((float) height);
            window_scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f/aspect_ratio, 1.0f, 1.0f));
        } else {
            float aspect_ratio = ((float) height)/((float) width);
            window_scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f/aspect_ratio, 1.0f));
        }

        // Set view to zoom out, centered by default at 0,0
        glm::mat4 camera_zoom_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(CAMERA_ZOOM, CAMERA_ZOOM, CAMERA_ZOOM));

        // Move the camera
        glm::mat4 camera_translation_matrix;
        camera_translation_matrix = glm::translate(glm::mat4(1.0f), -camera_pos);
    
        // Calculate the combined transformation matrix
        glm::mat4 view_matrix = window_scale_matrix * camera_zoom_matrix * camera_translation_matrix;


        // Render all game objects
        player->Render(view_matrix, current_time_);
        for (int i = 0; i < enemy_arr.size(); i++) {
            enemy_arr[i]->Render(view_matrix, current_time_);
        }
        for (int i = 0; i < projectile_arr.size(); i++) {
            projectile_arr[i]->Render(view_matrix, current_time_);
        }
        for (int i = 0; i < collectible_arr.size(); i++) {
            collectible_arr[i]->Render(view_matrix, current_time_);
        }
        for (int i = 0; i < gunner_projectile_arr.size(); i++) {
            gunner_projectile_arr[i]->Render(view_matrix, current_time_);
        }
        background->Render(view_matrix, current_time_);
    }


    /*** Handle Window Resizing ***/
    void Game::ResizeCallback(GLFWwindow* window, int width, int height) {
        // Set OpenGL viewport based on framebuffer width and height
        glViewport(0, 0, width, height);
    }


    /*** Load all the textures ***/
    void Game::LoadTextures(std::vector<std::string>& textures) {

        // Allocate a buffer for all texture references
        int num_textures = textures.size();
        tex_ = new GLuint[num_textures];
        glGenTextures(num_textures, tex_);

        // Load each texture
        for (int i = 0; i < num_textures; i++) {
            SetTexture(tex_[i], (resources_directory_g + textures[i]).c_str());
        }

        // Set first texture in the array as default
        glBindTexture(GL_TEXTURE_2D, tex_[0]);
    }


    /*** Set an objects texture ***/
    void Game::SetTexture(const GLuint& w, const char *fname) {

        // Bind texture buffer
        glBindTexture(GL_TEXTURE_2D, w);

        // Load texture from a file to the buffer
        int width, height;
        unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGBA);
        if (!image){
            std::cout << "Cannot load texture " << fname << std::endl;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);

        // Texture Wrapping, Tiled
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Texture Filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }


    /*** Print out the fps to the console (refactor later to display on-screen) ***/
    void Game::DisplayFPS(double dt) const {
        // avoid dividing by 0 
        if (dt > 0) {
            float fps = std::floor(1 / dt);
            if (FPS_CAP != 0 && fps > FPS_CAP) { fps = FPS_CAP; }
            std::cout << "Current Fps: " << fps << std::endl;
        }
    }


} // namespace game
