// Timer class definitions

#include "timer.h"

namespace game {

    /*** Constructor, initializes everything to default values ***/
    Timer::Timer(void) : start_time_(0.0), end_time_(0.0), running_(false) {}

    
    /*** Start the timer, can also be used to update the end time ***/ 
    void Timer::Start(float end_time) {
        start_time_ = glfwGetTime();
        end_time_ = start_time_ + end_time;
        running_ = true;
    }


    /*** Check if the timer has finished ***/
    bool Timer::Finished(void) {
        // if not running, return false
        if (!running_) {
            return false;
        }

        // if running, check if end_time_ has been surpassed
        double current_time = glfwGetTime();
        if (current_time >= end_time_) {
            running_ = false;
            return true;
        }

        // base case
        return false;
    }

} // namespace game