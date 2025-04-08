// Wave class declarations, Wave has enemy info, WaveControl has logic and array

#ifndef WAVES_H_
#define WAVES_H_

#include <iostream>
#include <vector>
#include "game_object.h"
#include "defs.h"
#include "derived_enemy_objects.h"

namespace game {

    // Wave Class -> Contains the enemies of which to spawn on a wave
    class Wave {
    public:
        // Constructor
        Wave(int gunner_count, int chaser_count, int kamikaze_count);

        // For debugging
        void Print() const;

        // Getters
        inline int GetGunnerCount(void) const { return gunner_count; }
        inline int GetChaserCount(void) const { return chaser_count; }
        inline int GetKamikazeCount(void) const { return kamikaze_count; }
        inline int GetEnemiesAlive(void) const { return enemies_alive; }

        // Decrement enemy counts
        inline void DecrementGunnerCount(void) { gunner_count--; }
        inline void DecrementChaserCount(void) { chaser_count--; }
        inline void DecrementKamikazeCount(void) { kamikaze_count--; }
        inline void DecrementEnemiesAlive(void) { enemies_alive--; }

    private :
        unsigned short int chaser_count, gunner_count, kamikaze_count, enemies_alive;
    };


    // WaveControl Class -> Holds all the waves to be played out in the game
    class WaveControl {
    public:
        // Constructor
        WaveControl();

        // Get which wave currently being played
        inline int GetCurrentWave(void) const { return current_wave + 1; }

        // Get the actual wave data for current wave
        inline Wave GetWave(void) const { return waves[current_wave]; }

        // Increment wave counter
        bool IncrementWave(void);

        // Decrement count of enemy based on parameter int
        void DecrementEnemyCount(int enemy_type);

        // Helpers for handling enemies left in the wave
        inline int EnemiesAlive(void) const { return waves[current_wave].GetEnemiesAlive(); }
        inline void EnemyExploded(void) { waves[current_wave].DecrementEnemiesAlive(); }

    private:
        std::vector<Wave> waves;
        unsigned short int current_wave;
        unsigned short int enemies_left_in_wave;

        const unsigned short int MAX_WAVES = 10;
    };

} // namespace game

#endif
