// audioRecord.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <math.h>
#include <complex>
#include <string.h>

using namespace std;

SDL_Window *gWindow = NULL;
SDL_Renderer *gRender = NULL;
SDL_Event _e;

SDL_AudioDeviceID gRecDevice;
SDL_AudioSpec gRecordSpec, gOutRecSpec;

SDL_Rect topViewport, bottomViewport, rightViewport;

const int maxRecord = 240000;

float windowingBuffer[256];
float signalBuffer[1024];
float soundBuffer[2000000];

int yMagnify = 100;
bool done = false;


Uint8 aStreamBuffer[2000000];
Uint8 recordingBuffer[16384];
int bufferPos;

void audioRecordCallback(void* userdata, Uint8* stream, int len)
{
    memcpy(&aStreamBuffer[bufferPos], stream, len);
    memcpy(&recordingBuffer, stream, len);
    bufferPos += len;
}

void programInit() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	gWindow = SDL_CreateWindow("yolo", 100, 100, 1280, 720, SDL_WINDOW_SHOWN);
	gRender = SDL_CreateRenderer(gWindow, 0, SDL_RENDERER_SOFTWARE);

	gRecordSpec.freq = 12000;
	gRecordSpec.format = AUDIO_F32;
	gRecordSpec.channels = 1;
	gRecordSpec.samples = 512;
	gRecordSpec.callback = audioRecordCallback;

	cout << "number of record device : " << SDL_GetNumAudioDevices(SDL_TRUE) << endl;

	int audioDeviceIndex = 0;
	gRecDevice = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(audioDeviceIndex, SDL_TRUE), SDL_TRUE, &gRecordSpec, &gOutRecSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
	SDL_PauseAudioDevice(gRecDevice, SDL_TRUE);

	cout << "format : " << gOutRecSpec.format << endl;
	cout << "freq sampling : " << gOutRecSpec.freq << endl;

	topViewport.x = 0;
	topViewport.y = 0;
	topViewport.w = 2048;
	topViewport.h = 400;
	bottomViewport.x = 0;
	bottomViewport.y = 250;
	bottomViewport.w = 256;
	bottomViewport.h = 400;
	rightViewport.x = 256;
	rightViewport.y = 250;
	rightViewport.w = 2048;
	rightViewport.h = 300;
}

int main(int argc, char** argv)
{
	//searchPitch();
	programInit();
	int crossingCount = 0;

	int nsample = gOutRecSpec.samples;

	//waiting input 'R'
	SDL_PauseAudioDevice(gRecDevice, SDL_TRUE);
	while (!done) {
		while (SDL_PollEvent(&_e))
		{
			if (_e.type == SDL_QUIT) {
				done = true;
			}
			if (_e.type == SDL_KEYDOWN) {
				if (_e.key.keysym.sym == SDLK_r) {
					done = true;
				}
			}
		}
	}

	//record until 10 second
	SDL_PauseAudioDevice(gRecDevice, SDL_FALSE);
	while (bufferPos < maxRecord)
	{
		while (SDL_PollEvent(&_e))
		{

		}
		float* bPointer = (float*) recordingBuffer;
		for (int i = 0; i < gRecordSpec.samples; i++) {
			signalBuffer[i] = bPointer[i];
		}

		SDL_SetRenderDrawColor(gRender, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(gRender);

		SDL_RenderSetViewport(gRender, &topViewport);
		SDL_SetRenderDrawColor(gRender, 0xff, 0x10, 0x10, SDL_ALPHA_OPAQUE);
		for (int i = 0; i < 256 - 1; i++) {
			SDL_RenderDrawLine(gRender, i, 200 - (int)(signalBuffer[i] * yMagnify), i + 1, 200 - (int)(signalBuffer[i + 1] * yMagnify));
		}

		float progressBar = ((float)bufferPos / (float) maxRecord) * 1280;
		cout << progressBar << endl;
		SDL_RenderDrawLine(gRender, 0, 100, (int)progressBar, 100);

		SDL_RenderPresent(gRender);
	}

	SDL_PauseAudioDevice(gRecDevice, SDL_TRUE);

	//process
	float* bPointer = (float*)aStreamBuffer;
	for (int i = 0; i < maxRecord/4; i++) {
		soundBuffer[i] = bPointer[i];
	}

	ofstream sf;
	sf.open("yolo.txt");
	for (int i = 0; i < maxRecord/4; i++) {
		sf << soundBuffer[i] << "\n";
	}

	sf.close();

	SDL_Quit();
	return 0;
}