// Game class definitions, defines all the main methods for the game

#define GLM_FORCE_RADIANS
#define GLEW_STATIC

#include "game.h"

// Directory with game resources such as textures
const std::string resources_directory_g = RESOURCES_DIRECTORY;

// Constants for the OpenGL window and viewport
const char* window_title_g = "Celestial Onslaught";
const unsigned int window_width_g = 900;
const unsigned int window_height_g = 600;
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
        delete particles_;

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

        // Scale to the monitor to make the window resolution agnostic
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE);

        // Create a window and its OpenGL context
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g, NULL, NULL);
        if (!window_) {
            glfwTerminate();
            throw(std::runtime_error(std::string("Could not create window")));
        }

        // Make the window's OpenGL context the current one
        glfwMakeContextCurrent(window_);

        // Store the Game instance in GLFWwindow's user data for callbacks
        glfwSetWindowUserPointer(window_, this);

        // Initialize window size with framebuffer size, not screen coordinates
        glfwGetFramebufferSize(window_, &window_width_, &window_height_);

        // Initialize the GLEW library to access OpenGL extensions
        // Need to do it after initializing an OpenGL context
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (err != GLEW_OK) {
            throw(std::runtime_error(std::string("Could not initialize the GLEW library: ") + std::string((const char*)glewGetErrorString(err))));
        }

        // Enable pre-multipled alpha blending so that transparency in .png files is shown correctly
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

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

        // Initialize tiling sprite geometry
        tiling_sprite_ = new Sprite();
        tiling_sprite_->CreateTilingGeometry();

        // Initialize particle geometry
        Particles* particles_temp = new Particles();
        particles_temp->CreateGeometry(PARTICLE_AMOUNT);
        particles_ = particles_temp;

        // Initialize sprite shader
        sprite_shader_.Init((resources_directory_g + std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/sprite_fragment_shader.glsl")).c_str());
        
        // Initialize text shader
        text_shader_.Init((resources_directory_g + std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/text_fragment_shader.glsl")).c_str());

        // Initialize particle shader
        particle_shader_.Init((resources_directory_g + std::string("/particle_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/particle_fragment_shader.glsl")).c_str());

        // Initialize time
        current_time_ = 0.0;

        // Start all the Timers
        enemy_spawn_timer.Start(ENEMY_SPAWN_DELAY);
        firing_cooldown.Start(PISTOL_SHOOT_CD);
        update_fps_timer.Start(FPS_UPDATE_INTERVAL);

        // Initialize default member variables
        update_flag = false;
        holding_shoot = false;
        holding_interact = false;
        camera_pos = glm::vec3(0.0f);
        camera_target_pos = glm::vec3(0.0f);
        cursor_pos = glm::vec3(0.0f);
        title_offset = glm::vec3(0.0f, 2.3f, 0.0f);
        spawn_index = 0;
        interact_id = 0;

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
            am.SetLoop(bg_music, true);

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
            filename = std::string(resources_directory_g).append("/audio/collect_power_up.wav");
            collect_sfx = am.AddSound(filename.c_str());
            am.SetSoundPosition(collect_sfx, 0.0, 0.0, 0.0);

            // Setup the ambience played by power ups
            filename = std::string(resources_directory_g).append("/audio/power_up_loop.wav");
            power_up_ambience = am.AddSound(filename.c_str());
            am.SetSoundPosition(power_up_ambience, 0.0, 0.0, 0.0);

            // Setup the player hit sound
            filename = std::string(resources_directory_g).append("/audio/player_hit.wav");
            player_hit_sfx = am.AddSound(filename.c_str());
            am.SetSoundPosition(player_hit_sfx, 0.0, 0.0, 0.0);

            // Setup the player shoot sound
            filename = std::string(resources_directory_g).append("/audio/player_shoot.wav");
            player_shoot_sfx = am.AddSound(filename.c_str());
            am.SetSoundPosition(player_shoot_sfx, 0.0, 0.0, 0.0);

            // Setup the enemy hit sound
            filename = std::string(resources_directory_g).append("/audio/enemy_hit.wav");
            enemy_hit_sfx = am.AddSound(filename.c_str());
            am.SetSoundPosition(enemy_hit_sfx, 0.0, 0.0, 0.0);

            // Setup the enemy shoot sound
            filename = std::string(resources_directory_g).append("/audio/enemy_shoot.wav");
            enemy_shoot_sfx = am.AddSound(filename.c_str());
            am.SetSoundPosition(enemy_shoot_sfx, 0.0, 0.0, 0.0);

            // Setup the wave complete sound
            filename = std::string(resources_directory_g).append("/audio/wave_complete.wav");
            wave_complete_sfx = am.AddSound(filename.c_str());
            am.SetSoundPosition(wave_complete_sfx, 0.0, 0.0, 0.0);

            // Setup the smg shot sound
            filename = std::string(resources_directory_g).append("/audio/smg_shoot.wav");
            smg_shoot_sfx = am.AddSound(filename.c_str());
            am.SetSoundPosition(smg_shoot_sfx, 0.0, 0.0, 0.0);

            // Setup the rifle shot sound
            filename = std::string(resources_directory_g).append("/audio/rifle_shoot.wav");
            rifle_shoot_sfx = am.AddSound(filename.c_str());
            am.SetSoundPosition(rifle_shoot_sfx, 0.0, 0.0, 0.0);

            // Setup the sniper shot sound
            filename = std::string(resources_directory_g).append("/audio/sniper_shoot.wav");
            sniper_shoot_sfx = am.AddSound(filename.c_str());
            am.SetSoundPosition(sniper_shoot_sfx, 0.0, 0.0, 0.0);

            // Setup the purchase sound
            filename = std::string(resources_directory_g).append("/audio/purchase.wav");
            purchase_sfx = am.AddSound(filename.c_str());
            am.SetSoundPosition(purchase_sfx, 0.0, 0.0, 0.0);

            // Setup the purchase sound
            filename = std::string(resources_directory_g).append("/audio/win_game.wav");
            win_game_sfx = am.AddSound(filename.c_str());
            am.SetSoundPosition(win_game_sfx, 0.0, 0.0, 0.0);

            // Set the master volume to a low value to avoid jumpscaring the listener
            am.SetMasterGain(MASTER_VOLUME);
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
            tex_bullet = 6,
            tex_portal = 7,
            tex_double_points = 8,
            tex_bullet_boost = 9,
            tex_cold_shock = 10,
            tex_font = 11,
            tex_vignette = 12,
            tex_dp_icon = 13,
            tex_bb_icon = 14,
            tex_cs_icon = 15,
            tex_orb = 16,
            tex_sawblade = 17,
            tex_base = 18,
            tex_link = 19,
            tex_smg_bullet = 20,
            tex_rifle_bullet = 21,
            tex_sniper_bullet = 22,
            tex_gunner_bullet = 23,
            tex_border = 24,
            tex_smg = 25,
            tex_rifle = 26,
            tex_sniper = 27,
            tex_kamikaze_explosion = 28,
            tex_armor_plating = 29,
            tex_regen_coating = 30,
            tex_nitro_infuse = 31,
            tex_celestial_augment = 32,
            tex_win_screen = 33
        };
        textures.push_back("/textures/player_ship.png");        // 0,  tex_player
        textures.push_back("/textures/gunner_ship.png");        // 1,  tex_gunner
        textures.push_back("/textures/chaser_ship.png");        // 2,  tex_chaser
        textures.push_back("/textures/kamikaze_ship.png");      // 3,  tex_kamikaze
        textures.push_back("/textures/background.png");         // 4,  tex_stars
        textures.push_back("/textures/explosion.png");          // 5,  tex_explosion
        textures.push_back("/textures/bullet.png");             // 6,  tex_bullet
        textures.push_back("/textures/enemy_spawn.png");        // 7,  tex_portal
        textures.push_back("/textures/double_points.png");      // 8,  tex_double_points
        textures.push_back("/textures/bullet_boost.png");       // 9,  tex_bullet_boost
        textures.push_back("/textures/cold_shock.png");         // 10, tex_cold_shock
        textures.push_back("/textures/font.png");               // 11, tex_font
        textures.push_back("/textures/vignette.png");           // 12, tex_vignette
        textures.push_back("/textures/dp_icon.png");            // 13, tex_dp_icon
        textures.push_back("/textures/bb_icon.png");            // 14, tex_bb_icon
        textures.push_back("/textures/cs_icon.png");            // 15, tex_cs_icon
        textures.push_back("/textures/orb.png");                // 16, tex_orb
        textures.push_back("/textures/sawblade.png");           // 17, tex_sawblade
        textures.push_back("/textures/base.png");               // 18, tex_base
        textures.push_back("/textures/link.png");               // 19, tex_link
        textures.push_back("/textures/smg_bullet.png");         // 20, tex_smg_bullet
        textures.push_back("/textures/rifle_bullet.png");       // 21, tex_rifle_bullet
        textures.push_back("/textures/sniper_bullet.png");      // 22, tex_sniper_bullet
        textures.push_back("/textures/gunner_bullet.png");      // 23, tex_gunner_bullet
        textures.push_back("/textures/buyable_border.png");     // 24, tex_border
        textures.push_back("/textures/buyable_smg.png");        // 25, tex_smg
        textures.push_back("/textures/buyable_rifle.png");      // 26, tex_rifle
        textures.push_back("/textures/buyable_sniper.png");     // 27, tex_sniper
        textures.push_back("/textures/kamikaze_explosion.png"); // 28, tex_kamikaze_explosion
        textures.push_back("/textures/armor_plating.png");      // 29, tex_armor_plating
        textures.push_back("/textures/regen_coating.png");      // 30, tex_regen_coating
        textures.push_back("/textures/nitro_infuse.png");       // 31, tex_nitro_infuse
        textures.push_back("/textures/celestial_augment.png");  // 32, tex_celestial_augment
        textures.push_back("/textures/win_screen.png");         // 33, tex_win_screen
        LoadTextures(textures);

        // Setup the player object (position, texture, vertex count)
        player = new PlayerGameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[tex_player]);

        // Setup 8 enemy spawn points on the outside of the map (represented by portal sprites)
        enemy_spawn_arr.push_back(new EnemySpawn(glm::vec3(16.0f, -9.0f, 0.0f), sprite_, &sprite_shader_, tex_[tex_portal]));
        enemy_spawn_arr.push_back(new EnemySpawn(glm::vec3(16.0f, 9.0f, 0.0f), sprite_, &sprite_shader_, tex_[tex_portal]));
        enemy_spawn_arr.push_back(new EnemySpawn(glm::vec3(9.0f, 16.0f, 0.0f), sprite_, &sprite_shader_, tex_[tex_portal]));
        enemy_spawn_arr.push_back(new EnemySpawn(glm::vec3(-9.0f, 16.0f, 0.0f), sprite_, &sprite_shader_, tex_[tex_portal]));
        enemy_spawn_arr.push_back(new EnemySpawn(glm::vec3(-16.0f, 9.0f, 0.0f), sprite_, &sprite_shader_, tex_[tex_portal]));
        enemy_spawn_arr.push_back(new EnemySpawn(glm::vec3(-16.0f, -9.0f, 0.0f), sprite_, &sprite_shader_, tex_[tex_portal]));
        enemy_spawn_arr.push_back(new EnemySpawn(glm::vec3(-9.0f, -16.0f, 0.0f), sprite_, &sprite_shader_, tex_[tex_portal]));
        enemy_spawn_arr.push_back(new EnemySpawn(glm::vec3(9.0f, -16.0f, 0.0f), sprite_, &sprite_shader_, tex_[tex_portal]));

        // Setup the HUD
        hud = new HUD(sprite_, &text_shader_, &sprite_shader_, tex_[tex_font], tex_[tex_dp_icon], tex_[tex_bb_icon], tex_[tex_cs_icon]);

        // Setup the title
        title = new TextGameObject(title_offset, sprite_, &text_shader_, tex_[tex_font]);
        title->SetText("Celestial Onslaught");

        // Setup the vignette (lighting effect)
        vignette = new GameObject(glm::vec3(0.0f), sprite_, &sprite_shader_, tex_[tex_vignette]);
        vignette->SetScale(glm::vec2(45.0f));

        // Setup background
        background = new GameObject(glm::vec3(0.0f), tiling_sprite_, &sprite_shader_, tex_[tex_stars]);
        background->SetScale(glm::vec2(WORLD_SIZE));

        // Setup Particles
        ParticleSystem* test_particle_system = new ParticleSystem(
            glm::vec3(0.0f, -0.45f, 0.0f), particles_, &particle_shader_, tex_[tex_orb], player
        );
        test_particle_system->SetScale(glm::vec2(0.2f));
        particle_system_arr.push_back(test_particle_system);


        /* Setup Buyable Areas */

        // SMG
        BuyableItem* smg_buy = new BuyableItem(glm::vec3(0.0f, 9.0f, 0.0f),
            sprite_, &sprite_shader_, tex_[tex_border], tex_[tex_smg]);
        buyable_arr.push_back(smg_buy);
        smg_buy->SetPointCost(3000);
        smg_buy->SetNameAndCost("'F' to buy SMG [" +
            std::to_string(smg_buy->GetPointCost()) + " Pts]");
        

        // Rifle
        BuyableItem* rifle_buy = new BuyableItem(glm::vec3(-14.0f, 14.0f, 0.0f),
            sprite_, &sprite_shader_, tex_[tex_border], tex_[tex_rifle]);
        buyable_arr.push_back(rifle_buy);
        rifle_buy->SetPointCost(4000);
        rifle_buy->SetNameAndCost("'F' to buy Rifle [" +
            std::to_string(rifle_buy->GetPointCost()) + " Pts]");

        // Sniper
        BuyableItem* sniper_buy = new BuyableItem(glm::vec3(14.0f, 14.0f, 0.0f),
            sprite_, &sprite_shader_, tex_[tex_border], tex_[tex_sniper]);
        buyable_arr.push_back(sniper_buy);
        sniper_buy->SetPointCost(5000);
        sniper_buy->SetNameAndCost("'F' to buy Sniper ["
            + std::to_string(sniper_buy->GetPointCost()) + " Pts]");

        // Armor Plating
        BuyableItem* ap_buy = new BuyableItem(glm::vec3(12.0f, -12.0f, 0.0f),
            sprite_, &sprite_shader_, tex_[tex_border], tex_[tex_armor_plating]);
        buyable_arr.push_back(ap_buy);
        ap_buy->SetPointCost(3000);
        ap_buy->SetNameAndCost("'F' to buy Armor Plating [" +
            std::to_string(ap_buy->GetPointCost()) + " Pts]");

        // Regen Coating
        BuyableItem* rc_buy = new BuyableItem(glm::vec3(-12.0f, -12.0f, 0.0f),
            sprite_, &sprite_shader_, tex_[tex_border], tex_[tex_regen_coating]);
        buyable_arr.push_back(rc_buy);
        rc_buy->SetPointCost(5000);
        rc_buy->SetNameAndCost("'F' to buy Regen Coating [" +
            std::to_string(rc_buy->GetPointCost()) + " Pts]");

        // Nitro-Infuse
        BuyableItem* ni_buy = new BuyableItem(glm::vec3(0.0f, -6.0f, 0.0f),
            sprite_, &sprite_shader_, tex_[tex_border], tex_[tex_nitro_infuse]);
        buyable_arr.push_back(ni_buy);
        ni_buy->SetPointCost(2500);
        ni_buy->SetNameAndCost("'F' to buy Nitro-Infuse [" +
            std::to_string(ni_buy->GetPointCost()) + " Pts]");

        // Celestial Augment
        BuyableItem* ca_buy = new BuyableItem(glm::vec3(0.0f, -17.0f, 0.0f),
            sprite_, &sprite_shader_, tex_[tex_border], tex_[tex_celestial_augment]);
        buyable_arr.push_back(ca_buy);
        ca_buy->SetPointCost(10000);
        ca_buy->SetNameAndCost("'F' to buy Celestial Augment [" +
            std::to_string(ca_buy->GetPointCost()) + " Pts]");


        /* Setup all the Weapons */

        pistol  = new Weapon
        (PISTOL_DMG,    PISTOL_SHOOT_CD,    PISTOL_LIFESPAN,    PISTOL_SPREAD,  PISTOL_SPEED,   PISTOL_SEMI);

        smg     = new Weapon
        (SMG_DMG,       SMG_SHOOT_CD,       SMG_LIFESPAN,       SMG_SPREAD,     SMG_SPEED,      SMG_SEMI   );

        rifle   = new Weapon
        (RIFLE_DMG,     RIFLE_SHOOT_CD,     RIFLE_LIFESPAN,     RIFLE_SPREAD,   RIFLE_SPEED,    RIFLE_SEMI );

        sniper  = new Weapon
        (SNIPER_DMG,    SNIPER_SHOOT_CD,    SNIPER_LIFESPAN,    SNIPER_SPREAD,  SNIPER_SPEED,   SNIPER_SEMI);

        // Set the default weapon (pistol)
        player->SetWeapon(pistol);

        // Set knockback cooldown on game start
        player->GetKnockbackCooldown().Start(1.0f);

        // Initialize win image
        winImage = new GameObject(glm::vec3(0, 0, 0), sprite_, &sprite_shader_, tex_[tex_win_screen]);
        winImage->SetScale(glm::vec2(0.0f));
    }


    /*** Destroy the game world, pseudo destructor ***/
    void Game::DestroyGameWorld(void) {

        // delete single pointer game objects
        delete player;
        delete background;
        delete vignette;
        delete hud;
        delete title;

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

        // delete enemy spawns
        for (int i = 0; i < enemy_spawn_arr.size(); ++i) {
            delete enemy_spawn_arr[i];
        }

        // delete buyables
        for (int i = 0; i < buyable_arr.size(); ++i) {
            delete buyable_arr[i];
        }

        // delete particles
        for (int i = 0; i < particle_system_arr.size(); ++i) {
            delete particle_system_arr[i];
        }

        // delete weapons
        delete pistol;
        delete smg;
        delete rifle;
        delete sniper;

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

            if (glfwWindowShouldClose(window_)) { break; }

            // from MainLoop()
            double current_time = glfwGetTime();
            double delta_time = current_time - last_time;
            last_time = current_time;

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

        // intro is done, so handle accordingly and run MainLoop (specified in main.cpp)
        update_flag = true;
        hud->SetHide(false);
        title->SetText(" ");
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

            // Spawn Enemies
            if (update_flag && enemy_arr.size() < MAX_ENEMIES) {
                if (enemy_spawn_timer.Finished()) {
                    SpawnEnemy();
                    enemy_spawn_timer.Start(ENEMY_SPAWN_DELAY);
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


    /*** Handle player inputs ***/
    void Game::HandleControls(double delta_time) {

        // Handle exiting the game (keys: ESC)
        if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == 1) {
            glfwSetWindowShouldClose(window_, true);
        }

        // Only handle input if necessary, i.e. game is still running
        if (update_flag) {

            // define local vars to help with motion
            glm::vec3 up(0.0f, 1.0f, 0.0f);
            glm::vec3 left(-1.0f, 0.0f, 0.0f);
            glm::vec3 accel(0.0f);
            float player_accel = player->GetAccelForce();

            // Handle physics-based movement input (keys: W, A, S, D)
            if (glfwGetKey(window_, GLFW_KEY_W) == 1) {
                accel += up * player_accel;
            }
            if (glfwGetKey(window_, GLFW_KEY_S) == 1) {
                accel -= up * player_accel;
            }
            if (glfwGetKey(window_, GLFW_KEY_A) == 1) {
                accel += left * player_accel;
            }
            if (glfwGetKey(window_, GLFW_KEY_D) == 1) {
                accel -= left * player_accel;
            }

            // Handle firing a bullet (mouse: LEFT-CLICK)
            if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == 1) {
                if (!holding_shoot && firing_cooldown.Finished()) {

                    Weapon* weapon = player->GetWeapon();

                    SpawnPlayerBullet();
                    firing_cooldown.Start(weapon->GetFiringCooldown());

                    // prevent semi-auto weapons from being shot by holding
                    if (weapon->IsSemiAuto()) {
                        holding_shoot = true;
                    }
                }
            }
            else {
                // Reset holding_shoot back to false if left-click isn't pressed
                holding_shoot = false;
            }

            // Handle the interact key (key: F)
            if (glfwGetKey(window_, GLFW_KEY_F) == 1) {
                if (!holding_interact) {
                    if (interact_id >= 1) {
                        HandlePlayerPurchase();
                    }
                    // ensure the player doesn't multi-buy
                    holding_interact = true;
                }
            }
            else {
                // Reset holding_interact back to false if the 'F' key isn't pressed
                holding_interact = false;
            }

            // Update player acceleration based on input
            player->SetAcceleration(accel);

            // Debug, kills the player instantly (keys: K)
            if (glfwGetKey(window_, GLFW_KEY_K) == 1) {
                KillPlayer();
            }
        }
    }


    /*** Handle the purchasing of a weapon or upgrade ***/
    void Game::HandlePlayerPurchase(void) {
        BuyableItem* buy_area = buyable_arr[interact_id - 1];

        if (player->GetPoints() >= buy_area->GetPointCost()
            && player->GetWeaponID() != interact_id) {

            int cost = buy_area->GetPointCost();
            am.PlaySound(purchase_sfx);
            switch (interact_id) {

            case 1:
                // handle buying the SMG
                player->SpendPoints(cost);
                player->SetWeapon(smg);
                player->SetWeaponID(1);
                player->DisableCelestialAugment();
                break;

            case 2:
                // handle buying the rifle
                player->SpendPoints(cost);
                player->SetWeapon(rifle);
                player->SetWeaponID(2);
                player->DisableCelestialAugment();
                break;

            case 3:
                // handle buying the sniper
                player->SpendPoints(cost);
                player->SetWeapon(sniper);
                player->SetWeaponID(3);
                player->DisableCelestialAugment();
                break;

            case 4:
                // handle buying the armor plating upgrade
                if (!player->IsArmorPlatingActive()) {
                    player->SpendPoints(cost);
                    player->EnableArmorPlating();
                }
                break;

            case 5:
                // handle buying the regen coating upgrade
                if (!player->IsRegenCoatingActive()) {
                    player->SpendPoints(cost);
                    player->EnableRegenCoating();
                }
                break;

            case 6:
                // handle buying the nitro-infuse upgrade
                if (!player->IsNitroInfuseActive()) {
                    player->SpendPoints(cost);
                    player->EnableNitroInfuse();
                }
                break;

            case 7:
                // handle buying the celestial augment upgrade
                if (!player->IsCelestialAugmentActive()) {
                    player->SpendPoints(cost);
                    player->EnableCelestialAugment();
                }
                break;

            default:
                // error handling
                std::cerr << "error with buying an item by index" << std::endl;
            }
        }
    }


    /*** Get the coordinates of the mouse cursor in the gameworld ***/
    void Game::UpdateCursorPosition(void) {

        // initialize cursor position (relative to monitor) and window size
        double mouse_x, mouse_y;
        glfwGetCursorPos(window_, &mouse_x, &mouse_y);

        // bounds check, abort if the mouse is outside the window
        if (mouse_x < 0 || mouse_x > window_width_ || mouse_y < 0 || mouse_y > window_height_) {
            return;
        }

        // cast width/height to float and initialize cursor position (relative to game world)
        float width = static_cast<float>(window_width_);
        float height = static_cast<float>(window_height_);

        // case 1: handle horizontal aspect ratio
        if (width >= height) {
            float aspect_ratio = width / height;
            cursor_pos.x = ((2.0f * mouse_x - width) * aspect_ratio) / (width * CAMERA_ZOOM);
            cursor_pos.y = (-2.0f * mouse_y + height) / (height * CAMERA_ZOOM);
        }

        // case 2: handle vertical aspect ratio
        else {
            float aspect_ratio = height / width;
            cursor_pos.x = (2.0f * mouse_x - width) / (width * CAMERA_ZOOM);
            cursor_pos.y = ((-2.0f * mouse_y + height) * aspect_ratio) / (height * CAMERA_ZOOM);
        }

        // convert to world coordinates by applying the camera offset
        cursor_pos += camera_pos;
    }


    /*** Update all the game objects, can change order by re-arranging functions ***/
    void Game::Update(double delta_time) {

        // visuals
        UpdateCamera(delta_time);
        vignette->SetPosition(player->GetPosition());
        title->SetPosition(camera_pos + title_offset);

        // player
        UpdatePlayer(delta_time);

        // fixed areas
        for (int i = 0; i < enemy_spawn_arr.size(); ++i) {
            enemy_spawn_arr[i]->Update(delta_time);
        }
        UpdateBuyables(delta_time);

        // enemies
        UpdateEnemies(delta_time);

        // projectiles
        UpdatePlayerProjectiles(delta_time);
        UpdateGunnerProjectiles(delta_time);

        // collectibles
        UpdateCollectibles(delta_time);
        
        // heads-up display
        UpdateHUD(delta_time);

    }


    /**********************************/
    /* SPECIFIC OBJECT UPDATERS BELOW */
    /**********************************/

    /*** Update the camera position accurately, with smooth movement and OOB prevention ***/
    void Game::UpdateCamera(double delta_time) {

        // apply linear interpolation movement (smooth-cam)
        camera_target_pos = player->GetPosition();
        glm::vec3 lerp_camera_pos = glm::mix(camera_pos, camera_target_pos, CAMERA_SMOOTHNESS);

        // calculate viewport-related variables
        float viewport_width = 2.0f * CAMERA_X_BOUND * CAMERA_ZOOM;
        float viewport_height = 2.0f * CAMERA_Y_BOUND * CAMERA_ZOOM;

        // adjust the viewport based on aspect ratio
        float aspect_ratio = static_cast<float>(window_width_) / window_height_;
        if (aspect_ratio > 1.0f) {
            viewport_width *= aspect_ratio;
        }
        else {
            viewport_height /= aspect_ratio;
        }

        // calculate dynamic bounds based on adjusted viewport
        float dynamic_x_bound = (WORLD_SIZE - viewport_width) * 0.5f;
        float dynamic_y_bound = (WORLD_SIZE - viewport_height) * 0.5f;

        // ensure bounds don't go negative (if viewport is larger than world)
        dynamic_x_bound = glm::max(dynamic_x_bound, 0.0f);
        dynamic_y_bound = glm::max(dynamic_y_bound, 0.0f);

        // clamp the camera position based on the dynamic bounds
        camera_pos.x = glm::clamp(lerp_camera_pos.x, -dynamic_x_bound, dynamic_x_bound);
        camera_pos.y = glm::clamp(lerp_camera_pos.y, -dynamic_y_bound, dynamic_y_bound);

    }


    /*** Update the player game object, ensures intended behavior every frame ***/
    void Game::UpdatePlayer(double delta_time) {
        if (update_flag) {
            if (player->GetHealth() <= 0) {
                KillPlayer();
            }
            else {
                // Update rotation based on cursor_pos
                glm::vec3 direction = cursor_pos - player->GetPosition();
                player->UpdateTargetAngle(atan2(direction.y, direction.x) - (HALF_PI));

                player->Update(delta_time);
                PlayerShotCheck(delta_time);
            }
        }
        else if (player->EraseTimerCheck()) {
            player->Hide();
        }
    }


    /*** Update the buyables and check if the player is in range to interact with them ***/
    void Game::UpdateBuyables(double delta_time) {
        
        // set defaults while checking if the player is in a buyable area
        hud->UpdateInfo("");
        bool player_in_radius = false;

        // iterate thru all of the buyables
        for (int i = 0; i < buyable_arr.size(); ++i) {
            BuyableItem* buy_area = buyable_arr[i];

            buy_area->Update(delta_time);

            // check if the player is in the buy radius
            if (CollisionCheck(buy_area, player) && !player_in_radius) {
                hud->UpdateInfo(buy_area->GetNameAndCost());
                interact_id = i+1;
                player_in_radius = true;
            }
        }

        // reset back to 0 if the player is not in a radius
        if (!player_in_radius) {
            interact_id = 0;
        }
    }


    /*** Update all the enemy game objects, ensures intended behavior every frame ***/
    void Game::UpdateEnemies(double delta_time) {
        for (int i = 0; i < enemy_arr.size(); ++i) {
            EnemyGameObject* enemy = enemy_arr[i];

            // Kamikaze D.O.T effect
            if (KamikazeEnemy* kamikaze = dynamic_cast<KamikazeEnemy*>(enemy)) {
                if (kamikaze->IsExplosionActive()) {
                    float dist = glm::length(player->GetPosition() - enemy->GetPosition());
                    if (dist < kamikaze->GetExplosionRadius()) {
                        player->TakeDamage(2); // or scale with delta_time for DoT
                    }
                }
            }

            if (enemy->EraseTimerCheck()) {
                enemy_arr.erase(enemy_arr.begin() + i);
                delete enemy;
            }

            // skip most updates if the enemy has exploded
            else if (!enemy->IsExploded()) {

                // check if the enemy has died
                if (enemy->GetHealth() <= 0) {
                    ExplodeEnemy(enemy);
                }
                else {

                    // if not dead, update according to the cold_shock flag
                    if (player->IsColdShockActive()) {
                        enemy->Update(delta_time / 2.5);
                    }
                    else {
                        enemy->Update(delta_time);
                    }

                    // misc updates to be called if the player is alive
                    if (update_flag) {

                        // check for gunner, only shoot if player is in range
                        if (GunnerEnemy* gunner = dynamic_cast<GunnerEnemy*>(enemy)) {
                            if (glm::distance(gunner->GetPosition(), player->GetPosition())
                                < GUNNER_SHOOT_DIST) {

                                if (gunner->IsShootCDFinished()) {
                                    SpawnGunnerBullet(gunner);
                                }
                            }
                        }
                        enemy->UpdateTarget(player);
                        EnemyShotCheck(enemy, delta_time);
                        EnemyCollisionCheck(enemy);
                    }
                }
            }
        }
    }


    /*** Update and process all the projectiles shot by the player ***/
    void Game::UpdatePlayerProjectiles(double delta_time) {
        for (int i = 0; i < projectile_arr.size(); ++i) {
            ProjectileGameObject* bullet = projectile_arr[i];

            bullet->Update(delta_time);

            if (bullet->EraseTimerCheck()) {
                projectile_arr.erase(projectile_arr.begin() + i);
                delete bullet;
            }
        }
    }


    /*** Update all the projectiles shot by the gunner enemies ***/
    void Game::UpdateGunnerProjectiles(double delta_time) {
        for (int i = 0; i < gunner_projectile_arr.size(); ++i) {
            ProjectileGameObject* bullet = gunner_projectile_arr[i];

            bullet->Update(delta_time);

            if (bullet->EraseTimerCheck()) {
                gunner_projectile_arr.erase(gunner_projectile_arr.begin() + i);
                delete bullet;
            }
        }
    }


    /*** Update all the collectibles (i.e. power-ups) ***/
    void Game::UpdateCollectibles(double delta_time) {
        for (int i = 0; i < collectible_arr.size(); ++i) {
            CollectibleGameObject* collectible = collectible_arr[i];

            if (collectible->EraseTimerCheck()) {
                collectible_arr.erase(collectible_arr.begin() + i);
                if (!collectible->IsCollected()) {
                    am.StopSound(power_up_ambience);
                }
                delete collectible;
            }

            else if (!collectible->IsCollected()) {
                collectible->Update(delta_time);

                if (CollisionCheck(player, collectible)) {
                    CollectPowerUp(collectible);
                }
            }
        }
    }


    /*** Update all the elements of the HUD ***/
    void Game::UpdateHUD(double delta_time) {

        // calculate corner positions based on window dimensions and camera zoom
        float world_width = 2.0f * CAMERA_X_BOUND * CAMERA_ZOOM;
        float world_height = 2.0f * CAMERA_Y_BOUND * CAMERA_ZOOM;

        // adjust for aspect ratio
        float aspect_ratio = static_cast<float>(window_width_) / window_height_;
        if (aspect_ratio > 1.0f) {
            world_width *= aspect_ratio;
        }
        else {
            world_height /= aspect_ratio;
        }

        // calculate positions relative to camera
        float rightEdge = camera_pos.x + (world_width * 0.5f) - (HUD_CORNER_MARGIN * 4);
        float leftEdge = camera_pos.x - (world_width * 0.5f) + (HUD_CORNER_MARGIN * 4);
        float topEdge = camera_pos.y + (world_height * 0.5f) - HUD_CORNER_MARGIN;
        float bottomEdge = camera_pos.y - (world_height * 0.5f) + HUD_CORNER_MARGIN;

        // bottom left corner
        hud->SetBottomLeftCorner(glm::vec3(leftEdge, bottomEdge, 0.0f));
        hud->UpdatePoints(std::to_string(player->GetPoints()));
        hud->UpdateEnemyCount(std::to_string(waves.EnemiesAlive()));
        hud->UpdateWave(std::to_string(waves.GetCurrentWave()));

        // bottom right corner
        hud->SetBottomRightCorner(glm::vec3(rightEdge, bottomEdge, 0.0f));
        hud->UpdatePowerUps(
            player->IsDoublePointsActive(),
            player->IsBulletBoostActive(),
            player->IsColdShockActive()
        );
        hud->UpdateHealth(std::to_string(player->GetHealth()));

        // top right corner, fps only, update based on FPS_UPDATE_INTERVAL
        hud->SetTopRightCorner(glm::vec3(rightEdge, topEdge, 0.0f));
        if (delta_time > 0 && update_fps_timer.Finished()) {
            update_fps_timer.Start(FPS_UPDATE_INTERVAL);
            int fps = std::floor(1 / delta_time);
            if (FPS_CAP != 0 && fps > FPS_CAP) {
                fps = FPS_CAP;
            }
            hud->UpdateFPS(std::to_string(fps));
        }

        // top left corner, time only
        hud->SetTopLeftCorner(glm::vec3(leftEdge, topEdge, 0.0f));
        hud->UpdateTime(glfwGetTime());

        // info segments, will hold buyable information, placeholders for now
        hud->SetMiddleBottom(glm::vec3(camera_pos.x, camera_pos.y - 1.5f, 0.0f));
    }

    /**********************************/
    /* SPECIFIC OBJECT UPDATERS ABOVE */
    /**********************************/


    /*** Check for collisions with the param enemy ***/
    void Game::EnemyCollisionCheck(EnemyGameObject* enemy) {
        if (ChaserEnemy* chaser = dynamic_cast<ChaserEnemy*>(enemy)) {
            if (CollisionCheck(player, chaser->GetChild1()) || CollisionCheck(player, chaser->GetChild2()) || CollisionCheck(player, chaser->GetChild3())) {
                glm::vec3 direction = glm::normalize(player->GetPosition() - chaser->GetPosition());
                player->ApplyKnockback(direction * 10.0f, chaser->GetDamage());
                // std::cout << "collided with arm segment" << std::endl;
                player->GetKnockbackCooldown().Start(1.0f);
            }
        }
        if (enemy && CollisionCheck(player, enemy)) {
            enemy->TakeDamage(enemy->GetHealth());
            player->TakeDamage(enemy->GetDamage());
            am.PlaySound(player_hit_sfx);
        }
    }


    /*** Explode an enemy game object ***/
    void Game::ExplodeEnemy(EnemyGameObject* enemy) {

        // change enemy properties to show it has exploded
        if (ChaserEnemy* chaser = dynamic_cast<ChaserEnemy*>(enemy)) {
            chaser->GetChild1()->SetScale(glm::vec2(0.0f));
            chaser->GetChild2()->SetScale(glm::vec2(0.0f));
            chaser->GetChild3()->SetScale(glm::vec2(0.0f));
            enemy->SetTexture(tex_[5]); // explosion texture
            enemy->SetScale(glm::vec2(1.8f));
        }
        // handle kamikaze explosion so as to activate the explosion and set it to a different texture
        else if (KamikazeEnemy* kamikaze = dynamic_cast<KamikazeEnemy*>(enemy)) {
            kamikaze->SetDamage(0);
            enemy->SetTexture(tex_[28]);
            enemy->SetScale(glm::vec2(3.0f));

            kamikaze->SetExplosionRadius(enemy->GetScale().x/2);
            kamikaze->ActivateExplosion();
        }
        else {
            enemy->SetTexture(tex_[5]); // explosion texture
            enemy->SetScale(glm::vec2(1.8f));
        }
        enemy->Explode();
        enemy->StartEraseTimer();

        // play explosion sound
        am.PlaySound(boom_sfx);

        // use RNG to decide if a power-up should spawn
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0f, 1.0f);

        // spawn if RNG gives a number less than the spawn chance
        if (dis(gen) <= POWER_UP_SPAWN_CHANCE) {
            SpawnCollectible(enemy);
        }

        // indicate in WaveControl that an enemy has been exploded
        waves.EnemyExploded();
    }


    /*** Check for ray-circle collision between an enemy and player-shot bullets ***/
    void Game::EnemyShotCheck(EnemyGameObject* enemy, double delta_time) {

        // check against all non-impacted bullets
        for (int i = 0; i < projectile_arr.size(); i++) {
            if (!projectile_arr[i]->GetImpact()) {
                ProjectileGameObject* bullet = projectile_arr[i];
                float collision_dist = enemy->GetXRadius() + BULLET_RADIUS;

                // check for ray-circle collision
                if (RayCircleCheck(bullet, enemy, collision_dist)) {
                    enemy->TakeDamage(player->GetDamage());
                    am.PlaySound(enemy_hit_sfx);

                    // enemy not alive, big point reward (assigned per enemy)
                    if (enemy->GetHealth() <= 0) {
                        player->AddPoints(enemy->GetPointReward());
                        ExplodeEnemy(enemy);
                    }

                    // enemy still alive, small point reward
                    else {
                        player->AddPoints(SHOT_HIT_POINT_REWARD);
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

                // check for ray-circle collision
                if (RayCircleCheck(bullet, player, collision_dist)) {
                    if (player->TakeDamage(bullet->GetDamage())) {
                        am.PlaySound(player_hit_sfx);
                    }
                }
            }
        }
    }


    /*** Handle collecting a collectible ***/
    void Game::CollectPowerUp(CollectibleGameObject* collectible) {
        collectible->Collect();

        // enable a power-up state depending on the power-up ID
        int power_up_id = collectible->GetPowerUpID();
        switch (power_up_id) {

        case 0:
            // ID = 0 : double points, all points received are doubled
            player->EnableDoublePoints();
            break;

        case 1:
            // ID = 1 : bullet boost, player deals triple damage
            player->EnableBulletBoost();
            break;

        case 2:
            // ID = 2 : cold shock, enemies become much slower
            player->EnableColdShock();
            break;
        }

        // handle sounds
        am.PlaySound(collect_sfx);
        am.StopSound(power_up_ambience);
    }


    /*** Spawn an enemy on a spawn portal according to the waves ***/
    void Game::SpawnEnemy(void) {
        // required definitions for function logic
        std::mt19937 gen(rd());
        int gunner = 1, chaser = 2, kamikaze = 3;
        std::vector<int> possible_enemies;
        EnemyGameObject* new_enemy = nullptr;

        // populate possible_enemies with the enemy types that can be spawned
        if (waves.GetWave().GetGunnerCount() > 0) {
            possible_enemies.push_back(gunner);
        }
        if (waves.GetWave().GetChaserCount() > 0) {
            possible_enemies.push_back(chaser);
        }
        if (waves.GetWave().GetKamikazeCount() > 0) {
            possible_enemies.push_back(kamikaze);
        }

        // if there are no more enemies to spawn, increment the wave
        if (waves.EnemiesAlive() <= 0) {
            if (waves.IncrementWave()) {
                std::cout << "Wave complete." << std::endl;
                am.PlaySound(wave_complete_sfx);
                enemy_spawn_timer.Start(2.0f);
            }
            else {
                int points = player->GetPoints();
                std::string rank = "";
                if (points < 15000) {
                    rank = "D";
                }
                if (points >= 15000 && points < 20000) {
                    rank = "C";
                }
                if (points >= 20000 && points < 25000) {
                    rank = "B";
                }
                if (points >= 25000 && points < 30000) {
                    rank = "A";
                }
                if (points >= 30000 && points < 35000) {
                    rank = "A+";
                }
                if (points >= 35000) {
                    rank = "S";
                }
                std::cout << "There are no more waves dawg. You Win!" << std::endl;
                std::cout << "Your rank is: " << rank << "." << std::endl;
                WinGame();
            }
            return;
        }

        // continue with spawning ONLY IF there are enemies remaining in the Wave object
        if (possible_enemies.size() > 0) {

            // get coordinates for the spawn based on spawn portals
            glm::vec3 spawn_pos = enemy_spawn_arr[spawn_index]->GetPosition();

            // handle spawn_index, specifies which portal to spawn at
            spawn_index++;
            if (spawn_index > 7) {
                spawn_index = 0;
            }

            // randomly decide which enemy to spawn based on possible_enemies vector
            std::uniform_int_distribution<> temp_dis(0, possible_enemies.size() - 1);
            int random_enemy_index = possible_enemies[temp_dis(gen)];
            switch (random_enemy_index) {

            case 1:
                // Spawn a Gunner, decrement the corresponding enemy counter
                new_enemy = new GunnerEnemy(spawn_pos, sprite_, &sprite_shader_, tex_[1]);
                waves.DecrementEnemyCount(gunner);
                break;

            case 2:
                // Spawn a Chaser, decrement the corresponding enemy counter
                new_enemy = new ChaserEnemy(spawn_pos, sprite_, &sprite_shader_, tex_[2], tex_[18], tex_[19], tex_[17]);
                waves.DecrementEnemyCount(chaser);
                break;

            case 3:
                // Spawn a Kamikaze, decrement the corresponding enemy counter
                new_enemy = new KamikazeEnemy(spawn_pos, sprite_, &sprite_shader_, tex_[3]);
                waves.DecrementEnemyCount(kamikaze);
                break;
            }

            // scale enemy health and speed based on current wave
            if (new_enemy) {
                if (waves.GetCurrentWave() == 4) {
                    new_enemy->SetHealthScale(1.2);
                }
                else if (waves.GetCurrentWave() == 6) {
                    new_enemy->SetHealthScale(1.2);
                    new_enemy->SetSpeedScale(1.05);
                }
                else if (waves.GetCurrentWave() == 8) {
                    new_enemy->SetHealthScale(1.5);
                    new_enemy->SetSpeedScale(1.1);
                }
                else if (waves.GetCurrentWave() == 10) {
                    new_enemy->SetHealthScale(1.75);
                    new_enemy->SetSpeedScale(1.2);
                }

                enemy_arr.push_back(new_enemy);
            }
        }
    }


    /*** Spawn a collectible, occasionally called when killing an enemy, spawns on their explosion ***/
    void Game::SpawnCollectible(EnemyGameObject* enemy) {
        std::mt19937 gen(rd()); // RNG setup

        // generate a random int to determine which power-up to spawn
        std::uniform_int_distribution<> dis_id(0, 2);
        int rand_id = dis_id(gen);

        // setup the new collectible
        CollectibleGameObject* power_up = new CollectibleGameObject(
            enemy->GetPosition(), sprite_, &sprite_shader_, tex_[8 + rand_id], 0 + rand_id
        );
        collectible_arr.push_back(power_up);

        // generate a random angle, then convert to a vector to be used as velocity
        std::uniform_real_distribution<> dis_angle(0.0f, TWO_PI);
        float velocity_angle = dis_angle(gen);

        // give the collectible random velocity
        power_up->SetVelocity(glm::vec3(cos(velocity_angle), sin(velocity_angle), 0.0f));

        // play an ambient sound to indicate the power-up has spawned
        am.PlaySound(power_up_ambience);
    }


    /*** Spawn a bullet at the player's position with a velocity/rotation corresponding to the player's ***/
    void Game::SpawnPlayerBullet(void) {

        // pre-definitions to reduce calls and make code prettier
        Weapon* weapon = player->GetWeapon();
        glm::vec3 player_pos = player->GetPosition();
        ProjectileGameObject* bullet;

        // create the bullet object and add to collection
        if (weapon == pistol) {
            bullet = new ProjectileGameObject(
                player_pos, sprite_, &sprite_shader_, tex_[6], weapon->GetBulletLifespan(), weapon->GetDamage()
            );
            am.PlaySound(player_shoot_sfx);
        }
        if (weapon == smg) {
            bullet = new ProjectileGameObject(
                player_pos, sprite_, &sprite_shader_, tex_[20], weapon->GetBulletLifespan(), weapon->GetDamage()
            );
            am.PlaySound(smg_shoot_sfx);
        }
        if (weapon == rifle) {
            bullet = new ProjectileGameObject(
                player_pos, sprite_, &sprite_shader_, tex_[21], weapon->GetBulletLifespan(), weapon->GetDamage()
            );
            am.PlaySound(rifle_shoot_sfx);
        }
        if (weapon == sniper) {
            bullet = new ProjectileGameObject(
                player_pos, sprite_, &sprite_shader_, tex_[22], weapon->GetBulletLifespan(), weapon->GetDamage()
            );
            am.PlaySound(sniper_shoot_sfx);
        }
        projectile_arr.push_back(bullet);

        // randomly generate spread based on the weapon's bullet spread member var
        std::random_device rd;
        std::mt19937 gen(rd());
        float spread = weapon->GetBulletSpread();
        std::uniform_real_distribution<> dis_spread(-spread, spread);
        float random_spread = dis_spread(gen);

        // calculate bullet direction with the spread offset
        glm::vec3 direction = glm::normalize(cursor_pos - player_pos);
        float base_angle = atan2(direction.y, direction.x);
        float spread_angle = base_angle + random_spread;
        glm::vec3 spread_direction = glm::vec3(cos(spread_angle), sin(spread_angle), 0);

        // apply the properties to the bullet
        bullet->SetVelocity(spread_direction * weapon->GetBulletSpeed());
        bullet->SetRotation(spread_angle - HALF_PI);

        // play the corresponding sound effect
    }


    /*** Spawn a bullet at the gunner's position with a velocity/rotation corresponding to the gunner's ***/
    void Game::SpawnGunnerBullet(GunnerEnemy* gunner) {

        // create the bullet object and add to collection
        ProjectileGameObject* bullet = new ProjectileGameObject(
            gunner->GetPosition(), sprite_, &sprite_shader_, tex_[6], GUNNER_BULLET_LIFESPAN, gunner->GetBulletDamage()
        );
        gunner_projectile_arr.push_back(bullet);

        // set appropriate physical properties
        glm::vec3 aim_line = player->GetPosition() - gunner->GetPosition();
        bullet->SetVelocity(glm::normalize(aim_line) * GUNNER_BULLET_SPEED);
        bullet->SetRotation(atan2(aim_line.y, aim_line.x) - (HALF_PI));

        // play the corresponding sound effect
        am.PlaySound(enemy_shoot_sfx);
    }


    /*** Handle Circle-Circle collision checking, only considers radius in the x-axis ***/
    bool Game::CollisionCheck(GameObject* obj_1, GameObject* obj_2) {
        return glm::length(obj_1->GetPosition() - obj_2->GetPosition())
               < obj_1->GetXRadius() + obj_2->GetXRadius();
    }


    /*** Return true if a Ray-Circle collision is found between a projectile and game object, false otherwise ***/
    bool Game::RayCircleCheck(ProjectileGameObject* bullet, GameObject* obj, float col_dist) {

        // perform ray-circle check via quadratic formula re-arrange
        glm::vec3 origin_to_center = bullet->GetOrigin() - obj->GetPosition();
        float a = glm::dot(bullet->GetVelocity(), bullet->GetVelocity());
        float b = 2.0f * glm::dot(origin_to_center, bullet->GetVelocity());
        float c = glm::dot(origin_to_center, origin_to_center) - col_dist * col_dist;

        // if discriminant is greater/equal zero, we have a collision
        float discriminant = b * b - 4 * a * c;
        if (discriminant >= 0) {

            // compute intersection points and check with the bullet's lifespan to confirm collision
            float t1 = (-b - glm::sqrt(discriminant)) / (2.0f * a);
            float t2 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
            float lifespan = bullet->GetBulletTimeElapsed();
            if (t1 <= lifespan && lifespan <= t2) {

                // collision confirmed, handle it, then return true
                bullet->ImpactOccured();
                bullet->Hide();
                return true;
            }
        }

        // base case, no collision found
        return false;
    }


    /*** Handle the player explosion once health hits 0 ***/
    void Game::KillPlayer() {

        // Player Specific
        player->SetTexture(tex_[5]); // explosion texture
        player->SetRotation(0);
        player->SetScale(glm::vec2(2.5f));
        player->StartEraseTimer();
        am.PlaySound(boom_sfx);

        // needed for later use
        std::string game_over_str = hud->CenterAlignString("GAME OVER!", SMALL_HUD_LEN);

        // Game Specific
        close_window_timer.Start(6.0f);
        update_flag = false;
        am.PlaySound(game_over_sfx);
    }


    /*** Closes the window, ending the gameplay ***/
    void Game::GameOver(void) {
        glfwSetWindowShouldClose(window_, true);
    }


    /*** Trigger Win Game ***/
    void Game::WinGame(void) {
        am.PlaySound(win_game_sfx);
        update_flag = false;
        close_window_timer.Start(6.0f);
        player->SetScale(glm::vec2(0.0f));
        winImage->SetPosition(player->GetPosition());
        winImage->SetScale(glm::vec2(10.0f));
    }

    /*** Render the Game World ***/
    void Game::Render(void) {

        /* Setup the view matrix and OpenGL settings appropriately */

        // Clear background
        glClearColor(
            viewport_background_color_g.r,
            viewport_background_color_g.g,
            viewport_background_color_g.b,
            0.0
        );
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use member variables for aspect ratio
        float aspect_ratio = static_cast<float>(window_width_) / window_height_;

        // Calculate window_scale_matrix using stored dimensions and aspect ratio
        glm::mat4 window_scale_matrix;
        if (aspect_ratio > 1.0f) {
            window_scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / aspect_ratio, 1.0f, 1.0f));
        }
        else {
            window_scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f / (1.0f / aspect_ratio), 1.0f));
        }

        // Set view to zoom out, centered by default at 0,0
        glm::mat4 camera_zoom_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(CAMERA_ZOOM, CAMERA_ZOOM, CAMERA_ZOOM));

        // Move the camera
        glm::mat4 camera_translation_matrix;
        camera_translation_matrix = glm::translate(glm::mat4(1.0f), -camera_pos);
    
        // Calculate the combined transformation matrix
        glm::mat4 view_matrix = window_scale_matrix * camera_zoom_matrix * camera_translation_matrix;

        // Transparent sprite helper
        glDepthMask(GL_FALSE);
        
        /* Render ALL the GameObjects in storage (order: back to front) */
        
        // Background
        background->Render(view_matrix, current_time_);

        for (int i = 0; i < enemy_spawn_arr.size(); ++i) {
            enemy_spawn_arr[i]->Render(view_matrix, current_time_);
        }

        for (int i = 0; i < buyable_arr.size(); ++i) {
            buyable_arr[i]->Render(view_matrix, current_time_);
            buyable_arr[i]->GetIcon()->Render(view_matrix, current_time_);
        }

        // Foreground
        glBlendFunc(GL_ONE, GL_ONE);
        for (int i = 0; i < particle_system_arr.size(); i++) {
            particle_system_arr[i]->Render(view_matrix, glfwGetTime());
        }
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        for (int i = 0; i < gunner_projectile_arr.size(); ++i) {
            gunner_projectile_arr[i]->Render(view_matrix, current_time_);
        }

        for (int i = 0; i < projectile_arr.size(); ++i) {
            projectile_arr[i]->Render(view_matrix, current_time_);
        }

        for (int i = 0; i < collectible_arr.size(); ++i) {
            collectible_arr[i]->Render(view_matrix, current_time_);
        }

        for (int i = 0; i < enemy_arr.size(); ++i) {
            enemy_arr[i]->Render(view_matrix, current_time_);
        }

        player->Render(view_matrix, current_time_);

        // Overlays
        vignette->Render(view_matrix, current_time_);

        winImage->Render(view_matrix, current_time_);
      
        title->Render(view_matrix, current_time_);

        hud->RenderAll(view_matrix, current_time_);


        // Set back to true, prevents the resize bug from occurring
        glDepthMask(GL_TRUE);
    }


    /*** Handle Window Resizing ***/
    void Game::ResizeCallback(GLFWwindow* window, int width, int height) {

        // Pause rendering until the resize is complete
        glfwMakeContextCurrent(window);
        glFinish();

        // Set OpenGL viewport based on framebuffer width and height
        glViewport(0, 0, width, height);

        // Update Game instance's window dimensions
        Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
        if (game) {
            game->window_width_ = width;
            game->window_height_ = height;
        }
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
        int width, height, channels;
        unsigned char* image = SOIL_load_image(fname, &width, &height, &channels, SOIL_LOAD_AUTO);

        // Error checking
        if (!image) {
            std::cout << "Cannot load texture " << fname << std::endl;
            return;
        }

        // Premultiply alpha if needed
        if (channels == 4) {
            PremultiplyAlpha(image, width * height);
        }

        // Determine format
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);

        // Texture Wrapping, Tiled
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Texture Filtering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }


    /*** Helper function for helping alpha channel rendering ***/
    void Game::PremultiplyAlpha(unsigned char* image, int pixelCount) {
        for (int i = 0; i < pixelCount; ++i) {
            float alpha = image[i * 4 + 3] / 255.0f;
            image[i * 4] = (unsigned char)(image[i * 4] * alpha);
            image[i * 4 + 1] = (unsigned char)(image[i * 4 + 1] * alpha);
            image[i * 4 + 2] = (unsigned char)(image[i * 4 + 2] * alpha);
        }
    }

} // namespace game
