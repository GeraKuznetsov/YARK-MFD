#pragma once

#include "SDL.h"
#include <string>

class Sound {
public:
	Uint32 wav_length; // length of our sample
	Uint8 *wav_buffer; // buffer containing our audio file
	SDL_AudioSpec wav_spec; // the specs of our piece of music
	Sound(std::string path);
	~Sound();
};

void OpenPlayer();
void PlayNext(Sound *s);
void PlaySound(Sound *ss);
