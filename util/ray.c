#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "ray.h"

//trig time

//need to check walls from state-HALF_FOV to state+half_FOV
//idea 1 naive check is to iterate through the world and check if the trig aligns
//idea 2 sweep by angle and check from player out to the first non 0, works because distance is needed for drawing anyways

struct hit * cast_rays(float theta, float x, float y, int world[LEVEL_HEIGHT][LEVEL_WIDTH], int num_rays, float phi){
	struct hit *results = malloc(sizeof(struct hit) * num_rays);

	float start_angle = theta - phi;
	float end_angle = theta + phi;
	float step = (end_angle - start_angle) / num_rays;
	for(int i = 0; i < num_rays; i++){
		float ang = start_angle + i * step;
		float dx = cosf(ang);
		float dy = sinf(ang);

		float working_dist = 0.0f; // for the tuple once the ray hits it
		int val = 0;
		int scanx, scany;
		while(working_dist < SCAN_MAX){
			scanx = (int)(x + dx *working_dist); //ray properties
			scany = (int)(y + dy *working_dist);
			if (scanx < 0 || scany < 0 || scanx >= LEVEL_WIDTH || scany >= LEVEL_HEIGHT) { //cond 1 out of bounds
				val = -1;
				break;
			}

			if (world[scany][scanx] && !(world[scany][scanx] == 99)){ //cond 2 hit
				val = world[scany][scanx];
				break;
			}

			working_dist += RAY_STEP;
		}
		if(working_dist > SCAN_MAX){ val = 0;}
		results[i].map_x = scanx;
		results[i].map_y = scany;
 		results[i].value = val;
		results[i].distance = working_dist;
	}
	return results;
}

struct hit * p_cast_rays(struct player *p, int world[LEVEL_HEIGHT][LEVEL_WIDTH], int num_rays){
	return cast_rays(p->state, p->x, p->y, world, num_rays, HALF_FOV);
}