#include "Sound.h"

#include <iostream>

Uint8 *audio_pos; // global pointer to the audio buffer to be played
Uint32 audio_len; // remaining length of the sample we have to play

//AUDIO_S16LSB
Sound::Sound(std::string path) {
	if (SDL_LoadWAV(path.data(), &wav_spec, &wav_buffer, &wav_length) == NULL) {
		std::cout << "load failed\n";
		return;
	}
}

Sound::~Sound() {
	SDL_FreeWAV(wav_buffer);
}

#include <vector>
std::vector <Sound*> playNext;

void my_audio_callback(void *userdata, Uint8 *stream, int len);

void OpenPlayer() {
	SDL_AudioSpec want;
	want.freq = 44100;
	want.format = AUDIO_S16LSB;
	want.channels = 2;
	want.samples = 4096;
	want.callback = my_audio_callback;
	want.userdata = NULL;
	if (SDL_OpenAudio(&want, NULL) < 0) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		exit(-1);
	}
	SDL_PauseAudio(0);
}

void PlayNext(Sound *s) {
	playNext.push_back(s);
}

void PlaySound(Sound *ss) {
	audio_pos = ss->wav_buffer; // copy sound buffer
	audio_len = ss->wav_length; // copy file length
}

void my_audio_callback(void *userdata, Uint8 *stream, int len) {
	if (audio_len == 0) {
		if (!playNext.empty()) {
			PlaySound(playNext[0]);
			playNext.erase(playNext.begin());
		}
		memset(stream, 0, len);
		return;
	}
	len = (len > audio_len ? audio_len : len);
	SDL_memcpy(stream, audio_pos, len); 					// simply copy from one buffer into the other
	//SDL_MixAudio(stream, audio_pos, len, 6);// mix from one buffer into another

	audio_pos += len;
	audio_len -= len;
}