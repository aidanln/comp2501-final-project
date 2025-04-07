// Wave class declarations, Wave has enemy info, WaveControl has logic and array

#ifndef WAVES_H_
#define WAVES_H_

#include <iostream>
#include <vector>
#include "game_object.h"
#include "defs.h"
#include "derived_enemy_objects.h"

namespace game {

    class Wave {
    public:
        // constructor for wave

        Wave(int gunner_count, int chaser_count, int kamikaze_count);
        // for debugging

        void print() const;

        // getters
        int GetGunnerCount() const { return gunner_count; }
        int GetChaserCount() const { return chaser_count; }
        int GetKamikazeCount() const { return kamikaze_count; }

        // decrement enemy counts
        void DecrementGunnerCount() { gunner_count--; }
        void DecrementChaserCount() { chaser_count--; }
        void DecrementKamikazeCount() { kamikaze_count--; }

    private :
        int chaser_count, gunner_count, kamikaze_count, total_count;
    };

    class WaveControl {
    public:
        WaveControl();
        // get which wave currently being played
        int GetCurrentWave() const { return current_wave + 1; }
        // get the actual wave data for current wave
        Wave GetWave() const { return waves[current_wave]; }
        // increment wave counter
        void IncrementWave() { current_wave++; }
        // decrement count of enemy based on parameter int
        void DecrementEnemyCount(int enemy_type);

    private:
        // stores waves
        std::vector<Wave> waves;
        // store current wave
        int current_wave;
    };


} // namespace game

#endif
