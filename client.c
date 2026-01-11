#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "util/ray.h"
#include "util/main.h"
#include <signal.h>
#include "net/networking.h"
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int player_id;
int hp = 100;
long long last;
int state = 0;
float x_max = 3.5f;
char* IP = "127.0.0.1";
int edit_world[64][64] = {0};
int test_arr[LEVEL_HEIGHT][LEVEL_WIDTH];
int score = 0;
long long last_enemy;
#define pellets 128


//NET

int server_socket = -1;

void cleanup(int signo) {
	if (server_socket != -1) {
		close(server_socket);
	}
	exit(0);
}

void send_player_state(int server_socket, int id, int shooting, int hp, float x, float y) {
	char buff[BUFFER_SIZE];
	snprintf(buff, BUFFER_SIZE, "%d %d %d %.2f %.2f", id, shooting, hp, x, y);
	int sent = send(server_socket, buff, strlen(buff), 0);
	printf("Sent %d bytes: %s\n", sent, buff);
}

void net_setup(){
	printf("Attempting to connect to %s...\n", IP);
	server_socket = client_tcp_handshake(IP);
	if (server_socket < 0) {
		printf("Failed to connect\n");
		exit(1);
	}
	printf("Connected to server on socket %d\n", server_socket);
}
//NET



void handle_input(struct player *p, float move_speed, float turn_speed, long long millis, const Uint8 *keystate, int *running) {
	if (keystate[SDL_SCANCODE_W]) {
		if(!test_arr[(int)(p->x + cosf(p->state) * move_speed)][(int)(p->y + sinf(p->state) * move_speed)]){
			p->x += cosf(p->state) * move_speed;
			p->y += sinf(p->state) * move_speed;
		}
	}
	if (keystate[SDL_SCANCODE_S]) {
		if(!test_arr[(int)(p->x - cosf(p->state) * move_speed)][(int)(p->y - sinf(p->state) * move_speed)]){
			p->x -= cosf(p->state) * move_speed;
			p->y -= sinf(p->state) * move_speed;
		}
	}
	if (keystate[SDL_SCANCODE_A]) {
		p->state -= turn_speed;
	}
	if (keystate[SDL_SCANCODE_D]) {
		p->state += turn_speed;
	}
	if (keystate[SDL_SCANCODE_SPACE]) {
		if(millis - last > 500){
			state = 1;
			last = millis;
			struct hit * bullet = cast_rays(p->state, p->x, p->y, test_arr, pellets, HALF_FOV);
			for(int i = 0; i < pellets; i++){
				if(bullet[i].value == 'E'){
					test_arr[bullet[i].map_y][bullet[i].map_x] = '@';
					score += 10;
					last_enemy = 0;
				}
			}
		}
	}
	if (keystate[SDL_SCANCODE_Q]) {
		*running = 0;
	}
	p->state = fmodf(p->state, 2 * M_PI);
}

void main_loop()
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
	p.x = 32;
	p.y = 32;
	p.state = 0;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	int running = 1;
	while(running){
		timespec_get(&ts, TIME_UTC);
		long long millis = (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

		SDL_Event e;
		while(SDL_PollEvent(&e)){
			if(e.type == SDL_QUIT) running = 0;
		}

		const Uint8 *keystate = SDL_GetKeyboardState(NULL);

		int num_rays = 128;
		struct hit * scan;
		scan = p_cast_rays(&p, test_arr, num_rays);

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		for (int i = 0; i < num_rays; i++) {
			float dy =  (SCREEN_HEIGHT / 2.0f) * x_max / fmaxf(scan[i].distance, 0.1f);
			int low = SCREEN_HEIGHT/2 - (int)dy;
			int high = SCREEN_HEIGHT/2 + (int)dy;
			int x = (i * SCREEN_WIDTH) / num_rays;
			if (low < 0) low = 0;
			if (high >= SCREEN_HEIGHT) high = SCREEN_HEIGHT - 1;
			if (x >= 0 && x < SCREEN_WIDTH && low <= high) {
				if (scan[i].value == 2) SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				else if (scan[i].value == 'E') SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				else SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);

				SDL_RenderDrawLine(renderer, x, low, x, high);
			}
		}

		if (hp <= 0) {
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderClear(renderer);
			SDL_RenderPresent(renderer);

			while(running) {
				while(SDL_PollEvent(&e)){
					if(e.type == SDL_QUIT) running = 0;
				}
				const Uint8 *ks = SDL_GetKeyboardState(NULL);
				if(ks[SDL_SCANCODE_Q]) running = 0;
				SDL_Delay(16);
			}
		}
        char recvbuf[BUFFER_SIZE];
		int n = recv(server_socket, recvbuf, BUFFER_SIZE, MSG_DONTWAIT);
		if (n > 0) {
			int other_id, other_state, other_hp;
			float other_x, other_y;
			sscanf(recvbuf, "%d %d %d %f %f", &other_id, &other_state, &other_hp, &other_x, &other_y);

			// Draw other player as red column
			float dx = other_x - p.x;
			float dy = other_y - p.y;
			float angle = atan2f(dy, dx) - p.state;
			angle = fmodf(angle + M_PI, 2 * M_PI) - M_PI;

			if (fabsf(angle) < HALF_FOV) {
				float dist = sqrtf(dx*dx + dy*dy);
				float col_height = (SCREEN_HEIGHT / 2.0f) * x_max / fmaxf(dist, 0.1f);
				int low = SCREEN_HEIGHT/2 - (int)col_height;
				int high = SCREEN_HEIGHT/2 + (int)col_height;
				int x = (int)((angle / HALF_FOV + 1.0f) * 0.5f * SCREEN_WIDTH);

				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
				SDL_RenderDrawLine(renderer, x, low, x, high);
			}
		}
		SDL_RenderPresent(renderer);
		free(scan);
		handle_input(&p, 1.0f, .1f, millis, keystate, &running);
		send_player_state(server_socket, player_id,state, hp, p.x, p.y);
		
		SDL_Delay((int)(1000.0f/FRAMERATE));
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char * argv[]){
	signal(SIGINT, cleanup);
	if(argc > 1) {
		IP = argv[1];
	}
	net_setup();
	player_id = getpid();
	main_loop();
	return 0;
}
