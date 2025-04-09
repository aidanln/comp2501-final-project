// Definitions for the Wave and WaveControl classes

#include "waves.h"

namespace game {

	/********************/
	/* Wave Definitions */
	/********************/

	/*** Contructor, initializes member vars and calcaultes enemies_alive ***/
	Wave::Wave(int gunner_count, int chaser_count, int kamikaze_count)
		: gunner_count(gunner_count), chaser_count(chaser_count), kamikaze_count(kamikaze_count) {
		enemies_alive = gunner_count + chaser_count + kamikaze_count;
	}


	/*** Print out the types of enemies remaining in a wave, used for debugging ***/
	void Wave::Print() const {
		std::cout << gunner_count << ", " << chaser_count << ", " << kamikaze_count << std::endl;
	}



	/***************************/
	/* WaveControl Definitions */
	/***************************/

	/*** Contructor, initializes all the waves and default values for member vars ***/
	WaveControl::WaveControl() {

		current_wave = 0;
		enemies_left_in_wave = 0;

		Wave wave1 (0, 8, 0);
		Wave wave2 (0, 16, 0);
		Wave wave3 (4, 12, 0);
		Wave wave4 (2, 16, 0);
		Wave wave5 (6, 12, 1);
		Wave wave6 (0, 20, 2);
		Wave wave7 (0, 0, 20);
		Wave wave8 (5, 25, 5);
		Wave wave9 (15, 15, 5);
		Wave wave10 (20, 30, 15);

		// needed for debugging win screen
		//current_wave = 9;
		//Wave wave10 (0, 0, 1);

		waves.insert(waves.end(), { wave1, wave2, wave3, wave4, wave5, wave6, wave7, wave8, wave9, wave10 });
	}


	/*** Decrease the count of an enemy type, indicating it has spawned ***/
	void WaveControl::DecrementEnemyCount(int enemy_type) {
		if (enemy_type == 1) {
			waves[current_wave].DecrementGunnerCount();
		}
		if (enemy_type == 2) {
			waves[current_wave].DecrementChaserCount();
		}
		if (enemy_type == 3) {
			waves[current_wave].DecrementKamikazeCount();
		}
		// Debug only
		// waves[current_wave].Print();
	}


	/*** Go to the next wave, avoid OOB errors, indicate success ***/
	bool WaveControl::IncrementWave(void) {
		if (current_wave < MAX_WAVES - 1) {
			current_wave++;
			return true;
		}
		// called if we are at the final wave
		return false;
	}
	
} // namespace game
