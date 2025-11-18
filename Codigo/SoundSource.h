#pragma once
#include <al.h>
class SoundSource
{
public:
	SoundSource();
	~SoundSource();

	void Play(const ALuint buffer_to_play);
	void SetLooping(bool loop);
	void SetVolume(float volume);
	void SetPosition(float x, float y, float z);
	void SetReferenceDistance(float distance);
	void SetMaxDistance(float distance);
	void SetRolloffFactor(float factor);

private:
	ALuint p_Source;
	float p_Pitch = 1.f;
	float p_Gain = 1.f;
	float p_Position[3] = { 0,0,0 };
	float p_Velocity[3] = { 0,0,0 };
	bool p_LoopSound = false;
	ALuint p_Buffer = 0;
};

