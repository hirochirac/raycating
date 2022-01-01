/*
 * main.h
 *
 *  Created on: 29 déc. 2021
 *      Author: b.Le Grin
 */

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include <time.h>
#include <math.h>

const int SCREEN_HEIGHT = 480;
const int SCREEN_WIDTH  = SCREEN_HEIGHT * 2;
const int MAP_SIZE      = 8;
const int TITLE_SIZE    = 32;
const float FOV         = 60.0;
const float HALF_FOV    = FOV / 2.0;
const int CASTED_RAYS   = 120;
const float STEP_ANGLE  = (float)(FOV / CASTED_RAYS );
const int MAX_DEPTH     = MAP_SIZE * TITLE_SIZE;

typedef struct {
	float radius;
	float x;
	float y;
	float a;
	float fov;
	float rot;
} Player;

/**
 *
 */
void displayMap (SDL_Renderer * renderer, int map[8][8]) {
	SDL_Rect rect;
	for (int i = 0; i < MAP_SIZE; ++i) {

		for (int j = 0; j < MAP_SIZE; ++j) {
			rect.h = TITLE_SIZE - 2;
			rect.w = TITLE_SIZE - 2;
			rect.x = i * TITLE_SIZE;
			rect.y = j * TITLE_SIZE;
            if ( map[j][i] == 1 ) {
            	SDL_SetRenderDrawColor(renderer,211,211,211,255);
            	SDL_RenderFillRect(renderer,&rect);
            	SDL_SetRenderDrawColor(renderer,0,0,0,0);
            }
		}
	}
}

/**
 *
 */
void displayPlayer (SDL_Renderer * renderer, Player * player, int map[8][8]) {
	SDL_SetRenderDrawColor(renderer,255,255,0,0);
	SDL_RenderDrawPoint(renderer,player->x,player->y);
	float degreeTotal = 360.0;
    float i = 0.0;
    while (i<=degreeTotal) {
    		SDL_RenderDrawPoint(renderer,
				            player->x + player->radius * cos((M_PI/180.0)*i),
				            player->y + player->radius * sin((M_PI/180.0)*i));
    		i += 0.01;
    }
	SDL_SetRenderDrawColor(renderer,0,0,0,0);
	SDL_SetRenderDrawColor(renderer,0,255,0,255);
	SDL_RenderDrawLine(renderer,
				       floor(player->x),
					   floor(player->y),
					   player->x - floor(50.0 * sin((M_PI/180.0) * player->a)),
					   player->y + floor(50.0 * cos((M_PI/180.0) * player->a)));
	SDL_RenderDrawLine(renderer,
					   floor(player->x),
					   floor(player->y),
					   player->x - floor(50.0 * sin((M_PI/180.0) * (player->a - HALF_FOV))),
					   player->y + floor(50.0 * cos((M_PI/180.0) * (player->a - HALF_FOV))));
	SDL_RenderDrawLine(renderer,
						   floor(player->x),
						   floor(player->y),
						   player->x - floor(50.0 * sin((M_PI/180.0) * (player->a + HALF_FOV))),
						   player->y + floor(50.0 * cos((M_PI/180.0) * (player->a + HALF_FOV))));


	SDL_SetRenderDrawColor(renderer,0,0,0,0);
}

/**
 *
 */
void displayRayCasting (SDL_Renderer * renderer, Player * player) {

	int row;
	int col;
	int depth = 0;
	float target_x = 0.0;
	float target_y = 0.0;
	float start_angle = player->a - (float) HALF_FOV;

	SDL_SetRenderDrawColor(renderer,0,255,0,255);
	for (int ray = 0; ray < 10; ++ray) {
		for (depth = 0; depth < 125; depth++) {
			target_x = player->x - sin((M_PI/180.0) * start_angle) * (float)depth;
			target_y = player->y + cos((M_PI/180.0) * start_angle) * (float)depth;


			//row = floor(target_x/TITLE_SIZE);
			//col = floor(target_y/TITLE_SIZE);

			SDL_RenderDrawLineF(renderer,
							       player->x,
								   player->y,
								   target_x,
								   target_y);
		}
		start_angle += STEP_ANGLE;
	}
	SDL_SetRenderDrawColor(renderer,0,0,0,0);
}

/**
 *
 */
int main (int argc, char** argv) {

	SDL_Window   * window   = NULL;
	SDL_Renderer * renderer = NULL;
	SDL_Event      event;
	int close               = 0;
	float elapseMS          = 0.0;
	Uint64 start, end;
	Player player;

	player.radius = 5.0;
    player.x      = TITLE_SIZE*8/2;
    player.y      = TITLE_SIZE*8/2;
    player.a      = 180.0;
    player.rot    = 0.0;
    player.fov    = 60.0;

    int map2[8][8] = {{1,1,1,1,1,1,1,1},
    				        {1,0,1,0,0,0,0,1},
    				        {1,0,1,0,0,0,0,1},
    				        {1,0,0,0,0,1,1,1},
    				        {1,0,0,0,0,0,0,1},
    			            {1,0,0,0,0,1,0,1},
    				        {1,0,0,0,0,1,0,1},
    				        {1,1,1,1,1,1,1,1}};

	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow("Raycasting", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	while (close == 0) {

		start = SDL_GetPerformanceCounter();
		end = start;

		SDL_RenderClear(renderer);
		while (SDL_PollEvent(&event)) {

			switch (event.type) {
				case SDL_QUIT : close = 1; break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.scancode) {
						case SDL_SCANCODE_ESCAPE:
							close = 1;
						    break;
						case SDL_SCANCODE_RIGHT:
							player.a+=0.9;
							break;
						case SDL_SCANCODE_LEFT:
							player.a-=0.9;
							break;
					}
					break;
			}

		}

		end = SDL_GetPerformanceCounter();

		displayMap(renderer,map2);
		displayPlayer(renderer,&player);
		displayRayCasting(renderer, &player, map2);
		elapseMS = (end-start) / (float) SDL_GetPerformanceFrequency() * 1000.0f;
		if (close == 0 )
			SDL_Delay(floor(16.666f-elapseMS));

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}

