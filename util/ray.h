#ifndef RAY_H
#define RAY_H
#include "main.h"
struct hit { //adhoc tuple
	float distance; 
	int value; 
	int map_x;
	int map_y;
};

#define SCAN_MAX 200.0f
#define RAY_STEP 0.5f
struct hit * cast_rays(float theta, float x, float y, int world[LEVEL_HEIGHT][LEVEL_WIDTH], int num_rays, float phi);
struct hit * p_cast_rays(struct player *p, int world[LEVEL_HEIGHT][LEVEL_WIDTH], int num_rays);
#endif