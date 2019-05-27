#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

#include <stdio.h>
#include <stdlib.h>
#include "assets.h"

//------------------- PARAMETERS --------------------//

// screen size
#define WIDTH  320
#define HEIGHT 240

// background color (RGB)
#define R 96
#define G 148
#define B 8

// delay until the logo starts being visible (unit: frames) (60 frames = 1 sec)
#define ANIMDELAY 60

// time from the moment the logo stops moving and sound is played until the logo app closes (unit: milliseconds)
#define ENDDELAY 3000

//speed at which the logo moves (unit: pixels per frame)
#define ANIMSPEED 1

//---------------------------------------------------//


int main(int argc, char* argv[]) {
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) {
		printf("Unable to initialize SDL: %s\n", SDL_GetError());
		return -1;
	}
	SDL_Surface *screen;

	screen = SDL_SetVideoMode(WIDTH, HEIGHT, 16, SDL_SWSURFACE);
	SDL_ShowCursor(SDL_DISABLE);
	SDL_RWops *RWops;
	SDL_Surface *logoimg;

#ifdef VERSION_POCKETGO
	RWops = SDL_RWFromMem(png_logo_pocketgo, sizeof(png_logo_pocketgo));
#else
	RWops = SDL_RWFromMem(png_logo_bittboy, sizeof(png_logo_bittboy));
#endif
    logoimg = IMG_LoadPNG_RW(RWops);
    if (!logoimg) {
    	printf("Error loading logo: %s\n", IMG_GetError());
    	return -1;
    }

    Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024);
	Mix_AllocateChannels(2);
	SDL_RWops *RWops2;
	Mix_Chunk *logosound;

	RWops2 = SDL_RWFromMem(wav_logosound, sizeof(wav_logosound));
    logosound = Mix_LoadWAV_RW(RWops2, 1);
    if (!logosound) {
    	printf("Error loading sound: %s\n", Mix_GetError());
    	return -1;
    }

	int dest_y = (screen->h - logoimg->h) / 2;
	uint32_t curr_time = SDL_GetTicks();
	uint32_t old_time = curr_time;
	uint32_t color = SDL_MapRGB(screen->format, R, G, B);
	SDL_Rect rect;
	SDL_Rect dstrect;
	for (int i = 0 - logoimg->h - ANIMDELAY; i <= dest_y; i = i + ANIMSPEED) {
		rect.x = 0;
		rect.y = 0;
		rect.w = screen->w;
		rect.h = screen->h;
		SDL_FillRect(screen, &rect, color);
		dstrect.x = (screen->w - logoimg->w) / 2;
		dstrect.y = i;
		dstrect.w = logoimg->w;
		dstrect.h = logoimg->h;
		SDL_BlitSurface(logoimg, NULL, screen, &dstrect);
		if (i == dest_y) {
			Mix_PlayChannel(-1, logosound, 0);
		}
		while (curr_time < old_time + 16) {
			curr_time = SDL_GetTicks();
		}
		old_time = curr_time;
		SDL_Flip(screen);
	}

	SDL_Delay(ENDDELAY);

	SDL_FreeRW(RWops);
	SDL_FreeRW(RWops2);
	SDL_FreeSurface(logoimg);
	Mix_FreeChunk(logosound);
	Mix_CloseAudio();
	SDL_Quit();

	return 0;
}
