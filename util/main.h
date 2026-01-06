#ifndef MAIN_H
#define MAIN_H

#define LEVEL_HEIGHT 64
#define LEVEL_WIDTH 64


#define HALF_FOV 0.78539816339f //pi/4



struct player{
	float x;
	float y;
	char * inv; // for later
	float state; //0 - 2PI.

};

struct enemy{
    float x;
    float y;
    int dmg;
    int hp;
    char face;
};

#endif
