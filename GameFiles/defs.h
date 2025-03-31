#ifndef DEFS_H_
#define DEFS_H_

		/*** External Constants ***/

// Toggle to have memory leaks printed on program exit
const bool PRINT_MEMORY_LEAKS = false;

// Macro for printing exceptions
#define PrintException(exception_object)\
    std::cerr << exception_object.what() << std::endl


namespace game {

		/*** Game Specific Constants ***/
	
	// How zoomed in the game should be
	const float CAMERA_ZOOM = 0.25f;

	// FPS Cap so that my laptop doesn't sound like a jet engine trying to hit 800fps unnecesarily
	const int FPS_CAP = 144;

	// Spawn-Related Flags and Timers
	const bool RANDOM_SPAWNING = true;
	const float ENEMY_SPAWN_DELAY = 3.0f;
	const float COLLECTIBLE_SPAWN_DELAY = 6.0f;

	// Collision thresholds
	const float COLLISION_DIST = 0.9f;
	const float BULLET_RADIUS = 0.4f;

	// Player related constants
	const float ACCEL_FORCE = 20.0f;
	const float MAX_SPEED = 4.2f;
	const unsigned short int STARTING_HP = 100;

	// Enemy related constants
	const float GUNNER_SPEED = 1.5f;
	const float ORBIT_RADIUS = 0.6f;
	const float ORBIT_SPEED = 4.0f;
	const float CHASER_SPEED = 2.5f;
	const float KAMIKAZE_SPEED = 4.0f;
	const float KAMIKAZE_FULL_SEND = 2.0f;

	// Bullet related constants
	const float BULLET_SPEED = 18.0f;
	const float GUNNER_BULLET_SPEED = 9.0f;
	const float BULLET_LIFESPAN = 0.4f;
	const float FIRING_COOLDOWN = 0.2f;

	// Gunner related constants
	const float BULLET_TIMER = 1.5f;
	const unsigned short int GUNNER_BULLET_DAMAGE = 5;

}

#endif
