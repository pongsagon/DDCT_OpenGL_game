#include "../_Def5.h"




#include <irrKlang/irrKlang.h>
irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();


struct B_Audio {
    const int irrKlang_Config = irrklang::ESEO_DEFAULT_OPTIONS;

    class AudioSource {

    private:
        std::vector<irrklang::ISound*> Sounds;
        std::vector<string> SoundNames;
        float Volume = 1.0;

    public:
        AudioSource() {
            Sounds.reserve(1024);
        }

        irrklang::ISound*
            Play(const char* AudioDIR)
        {
            irrklang::ISound* sound = SoundEngine->play2D(AudioDIR, false, false, true);

            if (sound) {
                SoundNames.push_back(AudioDIR);
                Sounds.push_back(sound);
                sound->setVolume(Volume);
                return sound;
            }
            return nullptr;
        }

        irrklang::ISound* GetSound() {
        }

        //MAX 1
        void SetVolume(float Target) {
            Volume = Target;
            for (irrklang::ISound* sound : Sounds) {
                if (sound) {
                    sound->setVolume(Volume);
                }
            }
        }

        void RemoveFinishedSounds() {
            Sounds.erase(std::remove_if(Sounds.begin(), Sounds.end(), [](irrklang::ISound* sound) {
                if (sound && sound->isFinished()) {
                    sound->drop();
                    return true; // Remove this sound from the vector
                }
                return false; // Keep this sound in the vector
                }), Sounds.end());
        }

        int GetActiveSounds() {
            return Sounds.size();
        }
    };

    AudioSource Music;
    AudioSource Ambient;
    AudioSource SoundEffects;

    void UpdateAllSource() {

        Music.RemoveFinishedSounds();
        Ambient.RemoveFinishedSounds();
        SoundEffects.RemoveFinishedSounds();
    }

}B_Audio;