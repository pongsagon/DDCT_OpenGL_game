#ifndef TIMER_H
#define TIMER_H

#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <limits>

class Timer {
public:
public:
    Timer(const glm::vec3& boxMin, const glm::vec3& boxMax)
        : running(false), hasStarted(false), startTime(0.0f), elapsedTime(0.0f), bestLapTime(std::numeric_limits<float>::max()), boxMin(boxMin), boxMax(boxMax) {}

    void start() {
        if (!running) {
            running = true;
            startTime = static_cast<float>(glfwGetTime());
        }
    }

    void stop() {
        if (running) {
            running = false;
            float lapTime = static_cast<float>(glfwGetTime()) - startTime;
            elapsedTime = lapTime; // Store the last lap's elapsed time

            // Update best lap time if applicable
            if (lapTime < bestLapTime) {
                bestLapTime = lapTime;
            }
        }
    }

    void reset() {
        running = false;
        startTime = 0.0f;
        elapsedTime = 0.0f;
    }

    void update(const glm::vec3& carPosition) {
        if (isCarInBox(carPosition)) {
            if (running) {
                stop();
                hasStarted = false; // Reset flag when the car re-enters the box
            }
        }
        else {
            // Start the timer only once when the car leaves the box for the first time
            if (!hasStarted) {
                start();
                hasStarted = true;
            }
        }

        // Update elapsed time if the timer is running
        if (running) {
            elapsedTime = static_cast<float>(glfwGetTime()) - startTime;
        }
    }

    std::string getFormattedTime() const {
        int totalMilliseconds = static_cast<int>(elapsedTime * 1000);
        int minutes = (totalMilliseconds / 60000);
        int seconds = (totalMilliseconds % 60000) / 1000;
        int milliseconds = totalMilliseconds % 1000;

        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%02d:%02d.%03d", minutes, seconds, milliseconds);
        return std::string(buffer);
    }

    std::string getBestLapTime() const {
        if (bestLapTime == std::numeric_limits<float>::max()) {
            return "00:00:000";
        }

        int totalMilliseconds = static_cast<int>(bestLapTime * 1000);
        int minutes = (totalMilliseconds / 60000);
        int seconds = (totalMilliseconds % 60000) / 1000;
        int milliseconds = totalMilliseconds % 1000;

        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%02d:%02d.%03d", minutes, seconds, milliseconds);
        return std::string(buffer);
    }

    bool isRunning() const {
        return running;
    }

private:

    bool isCarInBox(const glm::vec3& carPosition) const {
        return (carPosition.x >= boxMin.x && carPosition.x <= boxMax.x &&
            carPosition.y >= boxMin.y && carPosition.y <= boxMax.y &&
            carPosition.z >= boxMin.z && carPosition.z <= boxMax.z);
    }

    bool running;
    bool hasStarted; // Flag to track whether the timer/lap has started
    float startTime;
    float elapsedTime;
    float bestLapTime;
    glm::vec3 boxMin; //minimum corner of the box
    glm::vec3 boxMax; //max corner of the box

};

#endif
