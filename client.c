#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <fcntl.h>
#include "util/ray.h"
#include "util/main.h"
#include <signal.h>
#include "net/networking.h"
#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 480

//client player data
int player_id;
int hp = 100;
int score = 0;
int state = 0;

//reset logic
long long last;
long long last_enemy;
int killed = 0;


//gfx tuning
float x_max = 3.5f;

//PvE defaults
int ghosts_mode = 0;
float ghost_x = 32.0f;
float ghost_y = 32.0f;

//No args --> default IP
char* IP = "127.0.0.1";

//shotgun pellets
#define pellets 128

//static editting world and live world
static int edit_world[64][64] = {
	{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,2},
	{2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,2},
	{2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,2,0,0,0,0,0,2},
	{2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,2},
	{2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}
};
int world_arr[LEVEL_HEIGHT][LEVEL_WIDTH];







//NET
int server_socket = -1;

void cleanup(int signo) {
	if (server_socket != -1) {
		close(server_socket);
	}
	exit(0);
}

/*
   void send_player_state(int server_socket, int id, int shooting, int hp, float x, float y, float rot)

   Send all of the player data associated with this client to the connected server.
   */
void send_player_state(int server_socket, int id, int shooting, int hp, float x, float y, float rot) {
	char buff[BUFFER_SIZE];
	snprintf(buff, BUFFER_SIZE, "%d %d %d %f %f %f", id, shooting, hp, x, y, rot);
	int sent = send(server_socket, buff, strlen(buff), 0);
	//printf("Sent %d bytes: %s\n", sent, buff);
}

/*
   void net_setup(char * IP)

   Handshake wrapper.
   */

void net_setup(char * IP){
	server_socket = client_tcp_handshake(IP);
}


/*
   void wait_for_game_start()

   Keep players from playing until they get the 'Start' Keyword. Send client names to the server.
   */

void wait_for_game_start() {
	char name[32];
	printf("Enter name: ");
	fgets(name, 32, stdin);
	name[strcspn(name, "\n")] = 0;
	send(server_socket, name, strlen(name), 0);

	char buf[1024];
	while (1) {
		int n = recv(server_socket, buf, sizeof(buf) - 1, 0);
		if (n <= 0) exit(0);
		buf[n] = 0;
		printf("%s", buf);
		fflush(stdout);

		if (strstr(buf, "START")) {
			int flags = fcntl(server_socket, F_GETFL, 0);
			fcntl(server_socket, F_SETFL, flags | O_NONBLOCK);
			while (recv(server_socket, buf, sizeof(buf), 0) > 0); 
			fcntl(server_socket, F_SETFL, flags); // blocking
			break; 
		}
	}
}


/*
   void handle_input(struct player *p, float move_speed, float turn_speed, long long millis, const Uint8 *keystate, int *running)

   Takes the current player state and movement parameters and checks each keyboard event used in the game through the SDL window. Movement commands check for collision.
   Shooting command sets the state to 3 if a hit is detected, but otherwise this resets state each frame.
   */

void handle_input(struct player *p, float move_speed, float turn_speed, long long millis, const Uint8 *keystate, int *running) {
	state = 0;
	if (keystate[SDL_SCANCODE_W]) {
		//collision -- only 0 tiles are valid moves. This way anything else blocks: walls, enemies, corpses.
		if(!world_arr[(int)(p->y + sinf(p->rot) * move_speed)][(int)(p->x + cosf(p->rot) * move_speed)]){
			p->x += cosf(p->rot) * move_speed;
			p->y += sinf(p->rot) * move_speed;
		}
	}
	if (keystate[SDL_SCANCODE_S]) {
		if(!world_arr[(int)(p->y - sinf(p->rot) * move_speed)][(int)(p->x - cosf(p->rot) * move_speed)]){
			p->x -= cosf(p->rot) * move_speed;
			p->y -= sinf(p->rot) * move_speed;
		}
	}
	if (keystate[SDL_SCANCODE_A]) {
		p->rot -= turn_speed;
	}
	if (keystate[SDL_SCANCODE_D]) {
		p->rot += turn_speed;
	}
	if (keystate[SDL_SCANCODE_SPACE]) {
		if(millis - last > 500){
			state = 1;
			last = millis;
			struct hit * bullet = cast_rays(p->rot, p->x, p->y, world_arr, pellets, HALF_FOV);
			for(int i = 0; i < pellets; i++){
				if(bullet[i].value == 'E'){
					//world_arr[bullet[i].map_y][bullet[i].map_x] = '@'; //corpses
					state = 3;
				}
			}
		}
	}
	if (keystate[SDL_SCANCODE_Q]) {
		*running = 0;
	}
	p->rot = fmodf(p->rot, 2 * M_PI);
}
/*
   void main_loop()

   the main loop, called from main.....
   */
void main_loop()
{
	//init
	int s_x = 64;
	int s_y = 64;

	struct timespec ts;

	for (int x = 0; x < s_x; x++) {
		for (int y = 0; y < s_y; y++) {
			world_arr[y][x] = edit_world[y][x];
		}
	}

	struct player p;

	//dump the player in a random valid tile
	srand(getpid());
	p.x = rand() % 64;
	p.y = rand() % 64;
	while(world_arr[(int)p.y][(int)p.x] != 0) {
		p.x = rand() % 64;
		p.y = rand() % 64;
	}

	p.rot = 0;

	//SDL boiler
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	int running = 1;

	while(running){
		//SDL has its own time functions, but these are left over from the terminal game.
		timespec_get(&ts, TIME_UTC);
		long long millis = (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

		//check again for quits.
		SDL_Event e;
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_QUIT) running = 0;
		}

		//prep for void handle_input(...)
		const Uint8 *keystate = SDL_GetKeyboardState(NULL);

		//less rays, harder to see
		int num_rays = 512;

		//create and load this frames cast array
		struct hit * scan;
		scan = p_cast_rays(&p, world_arr, num_rays);

		//wipe the screen to draw next frame
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);


		//2d->3d
		for (int i = 0; i < num_rays; i++) {
			float dy =  (SCREEN_HEIGHT / 2.0f) * x_max / fmaxf(scan[i].distance, 0.1f);
			int low = SCREEN_HEIGHT/2 - (int)dy;
			int high = SCREEN_HEIGHT/2 + (int)dy;
			int x = (i * SCREEN_WIDTH) / num_rays;
			if (low < 0) low = 0;
			if (high >= SCREEN_HEIGHT) high = SCREEN_HEIGHT - 1;
			if (x >= 0 && x < SCREEN_WIDTH && low <= high) {
				if (scan[i].value == 2) SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				else if (scan[i].value == 'E'){ //check if dead 
					if(state ==3){
						SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
						SDL_RenderClear(renderer);
						SDL_RenderPresent(renderer);
					}else{
						SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
					}
				}
				else SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);

				SDL_RenderDrawLine(renderer, x, low, x, high);
			}
		}
		//death
		if (hp <= 0) {
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderClear(renderer);
			SDL_RenderPresent(renderer);
			usleep(2000000);
			exit(0);
		}
		//enemy pos.
		for(int i = 0; i<64; i++){
			for(int j = 0; j < 64; j++){
				if(world_arr[i][j] == 'E'){
					world_arr[i][j] = 0;
					break;
				}
			}
		}

		if(ghosts_mode) {
			float dx = p.x - ghost_x;
			float dy = p.y - ghost_y;
			float dist = sqrtf(dx*dx + dy*dy);
			//track players - valid moves check removed.
			//ghosts can go through walls, right?
			if(dist < 1.0f) {
				hp -= 50;
				ghost_x = rand() % 64;
				ghost_y = rand() % 64;
			} else {
				float step = 0.15f;
				ghost_x += (dx / dist) * step;
				ghost_y += (dy / dist) * step;
			}
			if(state == 3) {
				ghost_x = rand() % 64;
				ghost_y = rand() % 64;
				score += 10;
			}
			world_arr[(int)ghost_y][(int)ghost_x] = 'E';
		} else {
			char recvbuf[BUFFER_SIZE];
			int n = recv(server_socket, recvbuf, BUFFER_SIZE, MSG_DONTWAIT);
			int other_id, other_state, other_hp;
			float other_rot;
			float other_x, other_y;
			other_x = 0;
			if (n > 0 && other_x < 64) {
				sscanf(recvbuf, "%d %d %d %f %f %f", &other_id, &other_state, &other_hp, &other_x, &other_y, &other_rot);

				if(other_hp <= 0){
					killed = 1;
					state = 5;
					for(int i = 0; i < 64; i++){
						for(int j = 0; j < 64; j++){
							if(world_arr[i][j] == 'E'){
								world_arr[i][j] = 0;
								break;
							}
						}
					}
				} else {
					if(other_state == 3){
						hp -= 50;
					}			
					if(other_x < 64 && other_y < 64){
						world_arr[(int)other_y][(int)other_x] = 'E';
					}
				}
			}
		}

		// draw the gun onto the screen

		SDL_Surface *gun_surface = SDL_LoadBMP("gun.bmp");
		SDL_Texture *gun_texture = SDL_CreateTextureFromSurface(renderer, gun_surface);
		SDL_FreeSurface(gun_surface);

		SDL_Rect destG = {
			SCREEN_WIDTH-100,
			SCREEN_HEIGHT-100,
			100,
			100
		};

		SDL_RenderCopy(renderer, gun_texture, NULL, &destG);

		free(scan);
		handle_input(&p, 1.0f, .1f, millis, keystate, &running);

		//draw the text
		TTF_Font * sans = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
		SDL_Color White = {255,255,255};

		char message[100] = {0};

		snprintf(message, 100, ghosts_mode ? "Hp: %d Score: %d" : "Hp: %d pid: %d", hp, ghosts_mode ? score : getpid());

		SDL_Surface * surfaceMessage = TTF_RenderText_Solid(sans, message, White);
		SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);		      
		SDL_Rect destT = {
			100,
			100,
			300, 
			30 
		};

		SDL_RenderCopy(renderer, Message, NULL, &destT);
		if(!ghosts_mode) send_player_state(server_socket, player_id,state, hp, p.x, p.y, p.rot); //communicate this frame to the server
		SDL_RenderPresent(renderer);	
		SDL_DestroyTexture(Message);
		SDL_FreeSurface(surfaceMessage);
		TTF_CloseFont(sans);

		SDL_Delay((int)(1000.0f/FRAMERATE));
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char * argv[]){
	signal(SIGINT, cleanup);
	if(argc > 1) {
		if(strcmp(argv[1], "ghosts") == 0) {
			ghosts_mode = 1;
		} else {
			IP = argv[1];
		}
	}
	if(!ghosts_mode) {
		net_setup(IP);
		wait_for_game_start();
	}
	TTF_Init();
	player_id = getpid();
	main_loop();
	return 0;
}
