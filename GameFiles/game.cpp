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

    /*** Constructor, unused, replaced by Init() ***/
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


    /*** Initialize Game and OpenGL metadata ***/
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
        spawn_timer.Start(4.0f);
        collectible_timer.Start(7.5f);
        firing_cooldown.Start(0.5f);

        // Initialize default member variables
        update_flag = true;
        semi_auto = true;
        holding_shoot = false;
        player_last_pos = glm::vec3(0.0f);
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
            tex_bullet = 7
        };
        textures.push_back("/textures/player_ship.png");    // 0, tex_player
        textures.push_back("/textures/gunner_ship.png");    // 1, tex_gunner
        textures.push_back("/textures/chaser_ship.png");    // 2, tex_chaser
        textures.push_back("/textures/kamikaze_ship.png");  // 3, tex_kamikaze
        textures.push_back("/textures/background.png");     // 4, tex_stats
        textures.push_back("/textures/explosion.png");      // 5, tex_explosion
        textures.push_back("/textures/collectible.png");    // 6, tex_collectible
        textures.push_back("/textures/bullet.png");         // 7, tex_bullet
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

        // Game Setup is done, indicate with the start-up sound
        am.PlaySound(game_start_sfx);
        am.PlaySound(bg_music); // start bg music
    }


    /*** Destroy the game world, pseudo destructor ***/
    void Game::DestroyGameWorld(void) {
        // delete player
        delete player;
        // delete enemies
        for (int i = 0; i < enemy_arr.size(); ++i) {
            delete enemy_arr[i];
        }
        // delete projectiles
        for (int i = 0; i < projectile_arr.size(); ++i) {
            delete projectile_arr[i];
        }
        // delete collectibles
        for (int i = 0; i < collectible_arr.size(); ++i) {
            delete collectible_arr[i];
        }
        // shut down audio manager
        am.ShutDown();
    }


    /*** The main game loop, we stay stuck in here until the window closes ***/
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
            GetCursorPosition();

            // Handle user input
            HandleControls(delta_time);

            // Update all the game objects
            Update(delta_time);

            // Update the players last position to ensure proper camera functionality
            player_last_pos = player->GetPosition();

            // Check if timers are finished, unless a game over occurs
            if (update_flag && RANDOM_SPAWNING) {
                if (spawn_timer.Finished()) {
                    SpawnEnemy();
                    spawn_timer.Start(ENEMY_SPAWN_DELAY);
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

            // Enforce FPS cap, temporary solution till I find a workaround for my laptop
            while (1 / delta_time > FPS_CAP) {
                delta_time = glfwGetTime() - last_time;
            }
        }
    }


    /*** Get the coordinates of the mouse cursor in the gameworld ***/
    void Game::GetCursorPosition(void) {

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

        // Debug, kills the player instantly (keys: K)
        if (glfwGetKey(window_, GLFW_KEY_K) == 1) {
            KillPlayer();
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
                accel += up * ACCEL_FORCE;
            }
            if (glfwGetKey(window_, GLFW_KEY_S) == 1) {
                accel -= up * ACCEL_FORCE;
            }
            if (glfwGetKey(window_, GLFW_KEY_A) == 1) {
                accel += left * ACCEL_FORCE;
            }
            if (glfwGetKey(window_, GLFW_KEY_D) == 1) {
                accel -= left * ACCEL_FORCE;
            }

            // Handle firing a bullet (mouse: LEFT-CLICK)
            if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == 1) {
                // handle semi-auto weapons
                if (semi_auto && !holding_shoot) {
                    SpawnBullet();
                    holding_shoot = true;
                }
                // handle auto weapons
                else if (!semi_auto && firing_cooldown.Finished()) {
                    SpawnBullet();
                    firing_cooldown.Start(FIRING_COOLDOWN);
                }
            }
            else { holding_shoot = false; }

            // Update player acceleration based on input
            player->SetAcceleration(accel);
        }
    }


    /*** Update all the game objects ***/
    void Game::Update(double delta_time) {

        // Update the Player
        if (player->GetHealth() == 0 && update_flag) {
            KillPlayer();
        }
        if (update_flag) {
            player->UpdateTargetAngle(atan2(cursor_pos.y, cursor_pos.x) - (glm::pi<float>() / 2));
            player->Update(delta_time);
            PlayerBulletCollisionCheck(player, delta_time);
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
            else if (update_flag && !enemy->IsExploded()) {
                if (GunnerEnemy* gunner = dynamic_cast<GunnerEnemy*>(enemy)) {
                    if (gunner->IsFinished()) {
                        SpawnGunnerBullet(gunner);
                    }
                }
                enemy->UpdateTarget(player);
                enemy->Update(delta_time);
                BulletCollisionCheck(enemy, delta_time);
                EnemyCollisionCheck(enemy);
            }
        }

        // Update the Projectiles
        for (int i = 0; i < projectile_arr.size(); ++i) {
            ProjectileGameObject* bullet = projectile_arr[i];

            bullet->Update(delta_time);

            if (bullet->EraseTimerCheck()) {
                projectile_arr.erase(projectile_arr.begin() + i);
                delete bullet;
            }
        }

        // Update Gunner Projectiles
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
            else if (CollisionCheck(player, collectible, COLLISION_DIST) && !collectible->IsCollected()) {
                CollectItem(collectible);
            }
        }
    }


    /*** Check for collisions with the param enemy and update their state ***/
    void Game::EnemyCollisionCheck(EnemyGameObject* enemy) {

        // handle an enemy running into the player
        if (CollisionCheck(player, enemy, COLLISION_DIST)) {
            ExplodeEnemy(enemy);

            // update health, provided the player is vulnerable
            player->TakeDamage(enemy->GetDamage());
        }
    }


    /*** Check enemy for ray-circle collision ***/
    void Game::BulletCollisionCheck(EnemyGameObject* enemy, double delta_time) {

        // check against all non-impacted bullets
        for (int i = 0; i < projectile_arr.size(); i++) {
            if (!projectile_arr[i]->GetImpact()) {
                ProjectileGameObject* bullet = projectile_arr[i];

                // perform ray-circle check via quadratic formula re-arrange
                glm::vec3 origin_to_center = bullet->GetOrigin() - enemy->GetPosition();
                float a = glm::dot(bullet->GetVelocity(), bullet->GetVelocity());
                float b = 2.0f * glm::dot(origin_to_center, bullet->GetVelocity());
                float c = glm::dot(origin_to_center, origin_to_center) - BULLET_RADIUS * BULLET_RADIUS;

                // if discriminant is greater/equal zero, we have a collision
                float discriminant = b * b - 4 * a * c;
                if (discriminant >= 0) {

                    // compute intersection points and check with the bullet's lifespan to confirm collision
                    float t1 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
                    float t2 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
                    if (t1 <= bullet->GetLifespan() && bullet->GetLifespan() <= t2) {

                        // collision confirmed, handle it
                        bullet->ImpactOccured();
                        bullet->Hide();
                        ExplodeEnemy(enemy);
                    }
                }
            }
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


    /*** Check for player ray-circle collision ***/
    void Game::PlayerBulletCollisionCheck(PlayerGameObject* player, double delta_time) {

        // check against all non-impacted bullets
        for (int i = 0; i < gunner_projectile_arr.size(); i++) {
            if (!gunner_projectile_arr[i]->GetImpact()) {
                ProjectileGameObject* bullet = gunner_projectile_arr[i];

                // perform ray-circle check via quadratic formula re-arrange
                glm::vec3 origin_to_center = bullet->GetOrigin() - player->GetPosition();
                float a = glm::dot(bullet->GetVelocity(), bullet->GetVelocity());
                float b = 2.0f * glm::dot(origin_to_center, bullet->GetVelocity());
                float c = glm::dot(origin_to_center, origin_to_center) - BULLET_RADIUS * BULLET_RADIUS;

                // if discriminant is greater/equal zero, we have a collision
                float discriminant = b * b - 4 * a * c;
                if (discriminant >= 0) {

                    // compute intersection points and check with the bullet's lifespan to confirm collision
                    float t1 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
                    float t2 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
                    if (t1 <= bullet->GetLifespan() && bullet->GetLifespan() <= t2) {

                        // collision confirmed, handle it
                        bullet->ImpactOccured();
                        bullet->Hide();
                        player->TakeDamage(GUNNER_BULLET_DAMAGE);
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

        // Random Number Generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis_x(-6.0f, 6.0f);
        std::uniform_real_distribution<> dis_y(-4.0f, 4.0f);

        // generate the x and y coordinates, must be a certain distance from the player
        // LOWKEY UNOPTIMAL THIS MAKES TTC VERY RANDOM
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

        // Random Number Generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis_x(-7.0f, 7.0f);
        std::uniform_real_distribution<> dis_y(-4.6f, 4.6f);

        // generate the x and y coordinates, must be a certain distance from the player
        // LOWKEY UNOPTIMAL THIS MAKES FRAME TIMES VERY RANDOM
        glm::vec3 rand_pos(0.0f);
        while (glm::length(rand_pos) < 3.0f) {
            rand_pos.x = dis_x(gen);
            rand_pos.y = dis_y(gen);
        }

        // add a new collectible at the random position
        collectible_arr.push_back(new CollectibleGameObject(rand_pos, sprite_, &sprite_shader_, tex_[6]));
    }


    /*** Spawn a bullet at the player's position with a velocity/rotation corresponding to the player's ***/
    void Game::SpawnBullet(void) {

        // create the bullet object and add to collection
        ProjectileGameObject* bullet = new ProjectileGameObject(player->GetPosition(), sprite_, &sprite_shader_, tex_[7]);
        projectile_arr.push_back(bullet);
        bullet->StartEraseTimer();

        // set appropriate physical properties
        bullet->SetVelocity(glm::normalize(cursor_pos) * BULLET_SPEED);
        bullet->SetRotation(atan2(cursor_pos.y, cursor_pos.x) - (glm::pi<float>() / 2));

        // Ensures bullet comes out at the top of the player
        player->SetRotation(atan2(cursor_pos.y, cursor_pos.x) - (glm::pi<float>() / 2));

        // debug msg
        std::cout << "clicked at (" << cursor_pos.x << ", " << cursor_pos.y << ")" << std::endl;
    }


    /*** Spawn a bullet at the gunner's position with a velocity/rotation corresponding to the gunner's ***/
    void Game::SpawnGunnerBullet(GunnerEnemy* gunner) {

        // create the bullet object and add to collection
        ProjectileGameObject* bullet = new ProjectileGameObject(gunner->GetPosition(), sprite_, &sprite_shader_, tex_[7]);
        gunner_projectile_arr.push_back(bullet);
        bullet->StartEraseTimer();

        // set appropriate physical properties
        glm::vec3 aim_line = player->GetPosition() - gunner->GetPosition();
        bullet->SetVelocity(glm::normalize(aim_line) * GUNNER_BULLET_SPEED);
        bullet->SetRotation(atan2(aim_line.y, aim_line.x) - (glm::pi<float>() / 2));

        // debug msg
        std::cout << "gunner shoot towards (" << aim_line.x << ", " << aim_line.y << ")" << std::endl;
    }


    /*** Check if two objects have collided (using collision dist constant) ***/
    bool Game::CollisionCheck(GameObject* obj_1, GameObject* obj_2, float dist) {
        return glm::length(obj_1->GetPosition() - obj_2->GetPosition()) < dist;
    }


    /*** Handle the player explosion once health hits 0 ***/
    void Game::KillPlayer() {
        // Player Specific
        player->SetTexture(tex_[5]); // explosion texture
        player->SetRotation(0);
        player->SetScale(glm::vec2(2.5f));
        player->StartEraseTimer();
        if (!am.SoundIsPlaying(boom_sfx)) {
            am.PlaySound(boom_sfx); // do not play if an explosion is already playing
        }
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

        // move the camera with the player
        glm::mat4 camera_trans_matrix;
        if (update_flag) {
            camera_trans_matrix = glm::translate(glm::mat4(1.0f), -player->GetPosition());
        }
        else {
            camera_trans_matrix = glm::translate(glm::mat4(1.0f), -player_last_pos);
        }
    
        glm::mat4 view_matrix = window_scale_matrix * camera_zoom_matrix * camera_trans_matrix;


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

        // Texture Wrapping, now is tiled
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Texture Filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }


    /*** Print out the fps to the console (refactor later to display on-screen) ***/
    void Game::DisplayFPS(double dt) const {
        float fps = std::floor(1 / dt);
        if (fps > FPS_CAP) { fps = FPS_CAP; }
        std::cout << "Current Fps: " << fps << std::endl;
    }


} // namespace game
