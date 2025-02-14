#pragma once
#include <irrKlang/irrKlang.h>
#include <unordered_map>
#include <string>

using namespace irrklang;

class SoundManager {
public:
    SoundManager();
    ~SoundManager();

    void preloadSound(const std::string& name, const std::string& filepath);
    void playSound(const std::string& name, bool loop = false);
    void pauseSound(const std::string& name);
    void stopSound(const std::string& name);
    bool isPlaying(const std::string& name) const;
    void setVolume(const std::string& name, float volume);
    void setPlaybackSpeed(const std::string& name, float speed);

private:
    ISoundEngine* engine;
    std::unordered_map<std::string, ISoundSource*> soundSources; 
    std::unordered_map<std::string, ISound*> activeSounds; 
};
