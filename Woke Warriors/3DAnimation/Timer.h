#ifndef COUNTDOWN_TIMER_H
#define COUNTDOWN_TIMER_H

#include <GLFW/glfw3.h>
#include <string>
#include <iostream>

class CountdownTimer {
public:
    CountdownTimer(float initialTime = 60.0f, float delay = 0.0f)
        : running(false), startTime(0.0f), remainingTime(initialTime),
        defaultTime(initialTime), delay(delay), delayElapsed(0.0f) {}


    void start(float newDelay = -1.0f) {
        if (!running) {
            if (newDelay >= 0.0f) {
                delay = newDelay;
            }
            running = true;
            startTime = static_cast<float>(glfwGetTime());
            delayElapsed = 0.0f; // Reset delay tracker
        }
    }

    void stop() {
        if (running) {
            running = false;
            float elapsed = static_cast<float>(glfwGetTime()) - startTime;
            if (elapsed > delay) {
                remainingTime -= (elapsed - delay);
            }
            if (remainingTime < 0.0f) {
                remainingTime = 0.0f;
            }
        }
    }

    void restart() {
        running = false;
        remainingTime = defaultTime;
        delayElapsed = 0.0f;
    }

    void resetToDefault(float newTime = 60.0f) {
        running = false;
        defaultTime = newTime;
        remainingTime = defaultTime;
        delayElapsed = 0.0f;
    }


    void update() {
        if (running) {
            float currentTime = static_cast<float>(glfwGetTime());
            float elapsed = currentTime - startTime;

            // Handle delay
            if (delayElapsed < delay) {
                delayElapsed = elapsed;
                if (delayElapsed < delay) {
                    return; // Delay not yet finished
                }
            }

            remainingTime = defaultTime - (elapsed - delay);
            if (remainingTime <= 0.0f) {
                remainingTime = 0.0f;
                running = false;
            }
        }
    }

    std::string getFormattedTime() const {

        int totalSeconds = static_cast<int>(remainingTime);
        int seconds = totalSeconds % 60;

        // Convert seconds to a string dynamically (handles one or two digits)
        return std::to_string(seconds);

    }

    bool isRunning() const {
        return running;
    }

    bool isTimeUp() const {
        return remainingTime <= 0.0f;
    }

    float getRemainingTime() {
        return remainingTime;
    }

private:
    bool running;
    float startTime;      // When the timer started
    float remainingTime;  // Remaining time in seconds
    float defaultTime;    // Default countdown duration

    float delay;           // Delay before the countdown starts
    float delayElapsed;    // Time elapsed during the delay
};

#endif