#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include "util/L_term_gfx.h"
#include "util/ray.h"
#include "util/main.h"
#include "util/input.h"

int hp = 100;


long long last;
int shooting = 0;



float x_max = 3.5f;

int edit_world[64][64] = {0};
int test_arr[LEVEL_HEIGHT][LEVEL_WIDTH]; //the (test) world
int score = 0;
long long last_enemy;
#define pellets 128


void handle_input(struct player *p, float move_speed, float turn_speed, long long millis) {
	int ch = getchar();
	if (ch == -1 || ch == EOF) printf("return");

	//printf("Key: %c, pos: (%.2f, %.2f), angle: %.2f\n", ch, p->x, p->y, p->state);

	switch(ch) {
		case 'w':
			if(!test_arr[(int)(p->x +cosf(p->state) * move_speed)][(int)(p->y + sinf(p->state) * move_speed)]){
				p->x += cosf(p->state) * move_speed;
				p->y += sinf(p->state) * move_speed;
			}
			break;
		case 's':
			if(!test_arr[(int)(p->x -cosf(p->state) * move_speed)][(int)(p->y - sinf(p->state) * move_speed)]){
				p->x -= cosf(p->state) * move_speed;
				p->y -= sinf(p->state) * move_speed;
			}
			break;
		case 'a':
			p->state -= turn_speed;
			break;
		case 'd': 
			p->state += turn_speed;
			break;
		case ' ':
			if(millis-last > 500 ){
				shooting = 1;
				last = millis;
				struct hit * bullet = cast_rays(p->state, p->x, p->y, test_arr, pellets,HALF_FOV);
				for(int i = 0; i < pellets; i++){
					if(bullet[i].value == 'E'){
						test_arr[bullet[i].map_y][bullet[i].map_x] = '@';
						score += 10;
						last_enemy = 0;
					}
				}
			}

			break;
		case 'q': 
			raw_end(); 
			exit(0);

	}
	p->state = fmodf(p->state, 2 * M_PI);
}



void test_world_and_rays()
{

	int s_x = 64;
	int s_y = 64;
	struct timespec ts;


	for (int x = 0; x < s_x; x++) {
		for (int y = 0; y < s_y; y++) {
			test_arr[y][x] = edit_world[y][x];
		}
	}

	struct player p;
	p.x = 32; //init player in center
	p.y = 32;
	p.state = 0;

	struct frame f;
	f.id = 0;
	f.width = 80;
	f.height = 45;
	clear_frame(&f);
	struct frame * test_f = &f;


	raw_begin();
	while(1){
		timespec_get(&ts, TIME_UTC);
		long long millis = (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;


		int num_rays = 128;
		struct hit * scan;
		scan = p_cast_rays(&p, test_arr, num_rays);

		for (int i = 0; i < num_rays; i++) {
			float dy =  (f.height / 2.0f) * x_max / fmaxf(scan[i].distance, 0.1f);
			int low = f.height/2 - (int)dy;
			int high = f.height/2 + (int)dy;
			int x = (i * f.width) / num_rays;
			if (low < 0) low = 0;
			if (high >= f.height) high = f.height - 1;
			if (x >= 0 && x < f.width && low <= high) {
				draw_col_sing(test_f, (scan[i].value == 2) ? '#' : (scan[i].value == 'E' ? 'E' : '@'), x, low, high);
			}
		}

		//text






		// sprites

        //quits
		if (hp <= 0) {
			clear_frame(test_f);
			draw_text(test_f, "=== GAME OVER ===", f.width/2 - 8, f.height/2 - 2, 0xFF0000);
			draw_text(test_f, "You died.", f.width/2 - 4, f.height/2, 0xFFFFFF);
			draw_text(test_f, "Press Q to Quit", f.width/2 - 7, f.height/2 + 2, 0xFFFFFF);
			frame_draw(test_f);


			while(1) {
				int ch = getchar();
				if(ch == 'q') {
					raw_end();
					exit(0);
				}
			}
		}

        //draws
		frame_draw(test_f);
		free(scan);
		handle_input(&p, 1.0f, .1f,millis);
		usleep((int)(1000000 * (1.0f/FRAMERATE)));
	}
	free(test_f);

	usleep(1*1000000);


}


int main(){


	test_world_and_rays();
	raw_end();
	return 0;
}