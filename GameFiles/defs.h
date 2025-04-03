#ifndef DEFS_H_
#define DEFS_H_

#include <glm/gtc/constants.hpp>


		/*** External Constants ***/

// Toggle to have memory leaks printed on program exit
const bool PRINT_MEMORY_LEAKS = false;

// Macro for printing exceptions
#define PrintException(exception_object)\
    std::cerr << exception_object.what() << std::endl


namespace game {

		/*** Game Logic and Flow ***/

	// ensures device doesn't work itself super hard, set to 0 to uncap
	const unsigned short int FPS_CAP = 180;

	// Math constants
	const float PI = glm::pi<float>();
	const float HALF_PI = glm::half_pi<float>();
	const float TWO_PI = PI * 2.0f;

	// Camera constants, smoothness is just a lerp factor
	const float CAMERA_ZOOM = 0.25f;
	const float CAMERA_SMOOTHNESS = 0.1f;

	// How long the intro plays for
	const float INTRO_DURATION = 2.3f;



		/*** GameObject Spawning and Handling ***/

	// Spawn-Related Flags and Timers
	const bool RANDOM_SPAWNING = true;
	const float ENEMY_SPAWN_DELAY = 3.0f;
	const float COLLECTIBLE_SPAWN_DELAY = 6.0f;

	// constant collision thresholds
	const float BULLET_RADIUS = 0.1f;



		/*** PlayerGameObject Attributes ***/

	// Player general values
	const unsigned short int PLAYER_INIT_HP = 100;
	const float PLAYER_ACCEL_FORCE = 20.0f;
	const float PLAYER_MAX_SPEED = 4.2f;

	// Player healing helpers
	const float INVINCIBILITY_DURATION = 0.5f;
	const float REGEN_CD = 5.0f;
	const float REGEN_STEP_CD = 1.0f;
	const unsigned short int REGEN_AMOUNT = 10;



		/*** EnemyGameObject Attributes ***/

	// Gunner
	const unsigned short int GUNNER_INIT_HP = 20;
	const unsigned short int GUNNER_INIT_DMG = 40;
	const float GUNNER_SPEED = 0.5f;
	const float ORBIT_RADIUS = 0.6f;
	const float ORBIT_SPEED = 4.0f;
	const float GUNNER_STAY_DIST = 3.5f;
	const float GUNNER_EVADE_DIST = 2.5f;

	// Chaser
	const unsigned short int CHASER_INIT_HP = 15;
	const unsigned short int CHASER_INIT_DMG = 20;
	const float CHASER_SPEED = 2.5f;

	// Kamikaze
	const unsigned short int KAMIKAZE_INIT_HP = 5;
	const unsigned short int KAMIKAZE_INIT_DMG = 30;
	const float KAMIKAZE_SPEED = 4.0f;
	const float KAMIKAZE_LOCKON_DIST = 1.2f;

	// Gunner projectile specific
	const unsigned short int GUNNER_INIT_BULLET_DMG = 5;
	const float GUNNER_SHOOT_CD = 1.2f;
	const float GUNNER_BULLET_SPEED = 12.0f;
	const float GUNNER_BULLET_LIFESPAN = 1.0f;


		/*** Unupgraded Weapon Attributes ***/

	// Pistol
	const unsigned short int PISTOL_DMG = 3;
	const float PISTOL_SHOOT_CD = 0.2f;
	const float PISTOL_LIFESPAN = 0.4f;
	const float PISTOL_SPREAD = 0.15f;
	const float PISTOL_SPEED = 16.0f;
	const bool PISTOL_SEMI = true;

	// SMG
	const unsigned short int SMG_DMG = 5;
	const float SMG_SHOOT_CD = 0.075f;
	const float SMG_LIFESPAN = 0.3f;
	const float SMG_SPREAD = 0.3f;
	const float SMG_SPEED = 22.0f;
	const bool SMG_SEMI = false;

	// Rifle
	const unsigned short int RIFLE_DMG = 9;
	const float RIFLE_SHOOT_CD = 0.125f;
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
}

#endif
