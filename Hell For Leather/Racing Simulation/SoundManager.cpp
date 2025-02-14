#include "SoundManager.h"

SoundManager::SoundManager() {
    engine = createIrrKlangDevice();
    /*if (!engine) {
        throw std::runtime_error("Failed to initialize sound engine.");
    }*/
}

SoundManager::~SoundManager() {
    for (auto& pair : activeSounds) {
        ISound* sound = pair.second;
        if (sound) {
            sound->drop(); 
        }
    }
    engine->drop();  
}

void SoundManager::preloadSound(const std::string& name, const std::string& filepath) {
    ISoundSource* source = engine->addSoundSourceFromFile(filepath.c_str());
    if (source) {
        soundSources[name] = source;
    }
}

void SoundManager::playSound(const std::string& name, bool loop) {
    if (soundSources.find(name) != soundSources.end()) {
        ISound* sound = engine->play2D(soundSources[name], loop, false, true);
        if (sound) {
            activeSounds[name] = sound;  // Keep track of active sounds
        }
    }
}

void SoundManager::pauseSound(const std::string& name) {
    if (activeSounds.find(name) != activeSounds.end() && activeSounds[name]) {
        activeSounds[name]->setIsPaused(true);
    }
}

void SoundManager::stopSound(const std::string& name) {
    if (activeSounds.find(name) != activeSounds.end() && activeSounds[name]) {
        activeSounds[name]->stop();
        activeSounds[name]->drop();  // Release the sound
        activeSounds.erase(name);
    }
}

bool SoundManager::isPlaying(const std::string& name) const {
    if (activeSounds.find(name) != activeSounds.end() && activeSounds.at(name)) {
        return !activeSounds.at(name)->getIsPaused();
    }
    return false;
}

void SoundManager::setVolume(const std::string& name, float volume) {
    if (activeSounds.find(name) != activeSounds.end() && activeSounds[name]) {
        activeSounds[name]->setVolume(volume);
    }
}

void SoundManager::setPlaybackSpeed(const std::string& name, float speed) {
    if (activeSounds.find(name) != activeSounds.end() && activeSounds[name]) {
        activeSounds[name]->setPlaybackSpeed(speed);
    }
}
