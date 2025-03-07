#pragma once

#include <irrklang/irrKlang.h>
using namespace irrklang;

#include <string>
#include <unordered_map>

class Audio
{
public:
	Audio()
	{
		engine = createIrrKlangDevice();
	}

	void LoadSound(const std::string& name, const std::string& path)
	{
		if (ISoundSource* sound = engine->addSoundSourceFromFile(path.c_str(), ESM_AUTO_DETECT, true))
		{
			sound->setDefaultVolume(0.0f);
			engine->play2D(sound, false);
			sound->setDefaultVolume(1.0f);
			m_soundMap.insert({ name, sound });
		}
	}

	ISound* PlaySound(const std::string& name, bool loop = false)
	{
		if (m_soundMap.find(name) != m_soundMap.end())
			return engine->play2D(m_soundMap[name], loop, false, true);

		return nullptr;
	}

	void SetVolume(float volume)
	{
		engine->setSoundVolume(volume);
	}
	
private:
	std::unordered_map<std::string, ISoundSource*> m_soundMap;
	ISoundEngine* engine = nullptr;
};