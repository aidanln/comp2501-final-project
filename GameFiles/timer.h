// Timer class declarations, used often in the game for various time controls

#ifndef TIMER_H_
#define TIMER_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace game {

    class Timer {

        public:
            // Constructor and destructor
            Timer(void);
            ~Timer() {}

            // Start the timer now: end time given in seconds
            void Start(float end_time);

            // Check if timer has finished
            bool Finished(void) const;
            bool FinishedAndStop(void);

            // Check if timer is running
            inline bool IsRunning(void) const { return running_; }

        private:
            // Member vars
            double start_time_;
            double end_time_;
            bool running_;

    }; // class Timer

} // namespace game

#endif // TIMER_H_
