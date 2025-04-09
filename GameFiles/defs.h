// Contains Univeral Constants for the Game

#ifndef DEFS_H_
#define DEFS_H_


		/*** External Constants ***/

// Toggle to have memory leaks printed on program exit
const bool PRINT_MEMORY_LEAKS = false;

// Macro for printing exceptions
#define PrintException(exception_object)\
    std::cerr << exception_object.what() << std::endl


namespace game {


		/*** Game Logic and Flow ***/

	// Performance related
	const unsigned short int MAX_ENEMIES = 24;	// never exceed this many enemies spawned at once
	const unsigned short int FPS_CAP = 180;		// set to 0 to uncap
	const float FPS_UPDATE_INTERVAL = 0.333f;
	const float MASTER_VOLUME = 0.2f;			// 1.0 = max, 0.0 = muted
	const int PARTICLE_AMOUNT = 500;			// more particles leads to worse performance

	// Math constants
	const float PI = glm::pi<float>();
	const float HALF_PI = glm::half_pi<float>();
	const float TWO_PI = PI * 2.0f;

	// Camera and Game World Constants
	const float CAMERA_ZOOM = 0.25f;
	const float CAMERA_SMOOTHNESS = 0.05f;
	const float WORLD_SIZE = 40.0f;
	const float CAMERA_X_BOUND = 16.0f;
	const float CAMERA_Y_BOUND = 16.0f;
	const float PLAYER_X_BOUND = 19.5f;
	const float PLAYER_Y_BOUND = 19.5f;

	// Universal timers
	const float INTRO_DURATION = 5.4f;
	const float DEFAULT_ERASE_TIMER = 2.5f;
	const float ENEMY_SPAWN_DELAY = 1.5f;
	


		/*** PlayerGameObject Attributes ***/

	// Player general values
	const unsigned short int PLAYER_INIT_HP = 100;
	const float PLAYER_INIT_ACCEL = 20.0f;
	const float INIT_PLAYER_MAX_SPEED = 4.2f;
	const unsigned short int SHOT_HIT_POINT_REWARD = 10;

	// Player healing helpers
	const float INVINCIBILITY_DURATION = 0.15f;
	const float INIT_REGEN_CD = 4.0f;
	const float REGEN_STEP_CD = 0.1f;
	const unsigned short int INIT_REGEN_AMOUNT = 1;



		/*** CollectibleObject Attributes ***/

	const float COLLECTIBLE_SPEED = 0.25f;
	const float COLLECTIBLE_DURATION = 7.5f;
	const float POWER_UP_SPAWN_CHANCE = 0.025f; // 1/40 chance of power-up spawning
	const float POWER_UP_DURATION = 20.0f; // power up effects will last for 20s



		/*** EnemyGameObject Attributes ***/

	// Gunner
	const unsigned short int GUNNER_INIT_HP = 25;
	const unsigned short int GUNNER_INIT_DMG = 40;
	const unsigned short int GUNNER_POINT_REWARD = 250;
	const float GUNNER_SPEED = 0.5f;
	const float ORBIT_RADIUS = 0.6f;
	const float ORBIT_SPEED = 4.0f;
	const float GUNNER_STAY_DIST = 3.5f;
	const float GUNNER_EVADE_DIST = 2.5f;
	const float GUNNER_SHOOT_DIST = 5.5f;

	// Chaser
	const unsigned short int CHASER_INIT_HP = 15;
	const unsigned short int CHASER_INIT_DMG = 20;
	const unsigned short int CHASER_POINT_REWARD = 100;
	const float CHASER_SPEED = 2.5f;

	// Kamikaze
	const unsigned short int KAMIKAZE_INIT_HP = 5;
	const unsigned short int KAMIKAZE_INIT_DMG = 30;
	const unsigned short int KAMIKAZE_POINT_REWARD = 150;
	const float KAMIKAZE_SPEED = 4.0f;
	const float KAMIKAZE_LOCKON_DIST = 1.2f;

	// Gunner projectile specific
	const unsigned short int GUNNER_INIT_BULLET_DMG = 5;
	const float GUNNER_SHOOT_CD = 1.2f;
	const float GUNNER_BULLET_SPEED = 12.0f;
	const float GUNNER_BULLET_LIFESPAN = 1.0f;
	const float BULLET_RADIUS = 0.1f;



		/*** Unupgraded Weapon Attributes ***/

	// Pistol
	const unsigned short int PISTOL_DMG = 3;
	const float PISTOL_SHOOT_CD = 0.2f;
	const float PISTOL_LIFESPAN = 0.4f;
	const float PISTOL_SPREAD = 0.15f;
	const float PISTOL_SPEED = 16.0f;
	const bool PISTOL_SEMI = true;

	// SMG
	const unsigned short int SMG_DMG = 2;
	const float SMG_SHOOT_CD = 0.1f;
	const float SMG_LIFESPAN = 0.3f;
	const float SMG_SPREAD = 0.3f;
	const float SMG_SPEED = 22.0f;
	const bool SMG_SEMI = false;

	// Rifle
	const unsigned short int RIFLE_DMG = 4;
	const float RIFLE_SHOOT_CD = 0.16f;
	const float RIFLE_LIFESPAN = 0.5f;
	const float RIFLE_SPREAD = 0.1f;
	const float RIFLE_SPEED = 26.0f;
	const bool RIFLE_SEMI = false;

	// Sniper
	const unsigned short int SNIPER_DMG = 30;
	const float SNIPER_SHOOT_CD = 0.8f;
	const float SNIPER_LIFESPAN = 0.7f;
	const float SNIPER_SPREAD = 0.01f;
	const float SNIPER_SPEED = 35.0f;
	const bool SNIPER_SEMI = true;



		/*** HUD and Text Constants ***/

	// size of hud element areas
	const float CORNER_TEXT_SIZE_X = 3.2f;
	const float CORNER_TEXT_SIZE_Y = 0.4f;
	const float INFO_TEXT_SIZE_X = 7.1f;
	const float INFO_TEXT_SIZE_Y = 0.3f;
	const float ICON_SIZE = 0.6f;

	// Margins
	const float HUD_CORNER_MARGIN = 0.5f;

	// char limits
	const unsigned int SMALL_HUD_LEN = 19;
	const unsigned int LONG_HUD_LEN = 40;

}

#endif
