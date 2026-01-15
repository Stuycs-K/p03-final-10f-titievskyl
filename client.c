#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "util/ray.h"
#include "util/main.h"
#include <signal.h>
#include "net/networking.h"
#define SCREEN_WIDTH 620
#define SCREEN_HEIGHT 480

int player_id;
int hp = 100;
long long last;
int state = 0;
float x_max = 3.5f;
int killed = 0;



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

void send_player_state(int server_socket, int id, int shooting, int hp, float x, float y, float rot) {
	char buff[BUFFER_SIZE];
	snprintf(buff, BUFFER_SIZE, "%d %d %d %f %f %f", id, shooting, hp, x, y, rot);
	int sent = send(server_socket, buff, strlen(buff), 0);
	printf("Sent %d bytes: %s\n", sent, buff);
}

void net_setup(int passed_socket){
    if (passed_socket >= 0) {
        server_socket = passed_socket;
    } else {
        server_socket = client_tcp_handshake(IP);
    }
}
//NET
void wait_for_game_start() {
    char name[32];
    printf("Enter name: ");
    fgets(name, 32, stdin);
    name[strcspn(name, "\n")] = 0;
    send(server_socket, name, strlen(name), 0);
    
    char buf[256];
    while (1) {
        int n = recv(server_socket, buf, 255, 0);
        if (n > 0) {
            buf[n] = 0;
            printf("%s", buf);
            fflush(stdout);
            if (strstr(buf, "START")) break;
        }
    }
}




void handle_input(struct player *p, float move_speed, float turn_speed, long long millis, const Uint8 *keystate, int *running) {
	state = 0;
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
					//test_arr[bullet[i].map_y][bullet[i].map_x] = '@'; //corpses
					score += 10;
					last_enemy = 0;
					state = 3;
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
	//does this need to be redef'd? trace....
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

		int num_rays = 512;
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
				else if (scan[i].value == 'E'&&!killed){ //check if dead 
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
			p.x = 1000;
			usleep(500000);
			exit(0);
		}



		//enemy pos.
		char recvbuf[BUFFER_SIZE];
		int n = recv(server_socket, recvbuf, BUFFER_SIZE, MSG_DONTWAIT);
		int other_id, other_state, other_hp;
                        float other_rot;
                        float other_x, other_y;
			other_x = 0; // dep from teleporter death method
		if (n > 0 && other_x < 64) {
			//parse
			sscanf(recvbuf, "%d %d %d %f %f %f", &other_id, &other_state, &other_hp, &other_x, &other_y, &other_rot);
			for(int i = 0; i<64; i++){
				for(int j = 0; j < 64; j++){
					if(test_arr[i][j] == 'E'){
						test_arr[i][j] = 0; //draw over old pos. n enemy packets = n resets, only one recv per cycle.
						break;
					}
				}
			}
			if(other_hp==0){
				killed = 1;
				state = 5;
			}
			if(other_state == 3){
				hp -= 50;
			}			
			//round position for shooting later
			if(other_x < 64 && other_y < 64){
				test_arr[(int)other_y][(int)other_x] = 'E';
			}
			//texture setup
			/*
			SDL_Surface *sprite_surface = SDL_LoadBMP("enemy.bmp");
			SDL_Texture *enemy_texture = SDL_CreateTextureFromSurface(renderer, sprite_surface);
			SDL_FreeSurface(sprite_surface);

			//scaling and pos
			float dx = other_x - p.x;
			float dy = other_y - p.y;
			float angle = atan2f(dy, dx) - p.state;
			angle = fmodf(angle + M_PI, 2 * M_PI) - M_PI;

			if (fabsf(angle) < HALF_FOV) {
				float dist = sqrtf(dx*dx + dy*dy);
				int sprite_height = (int)((SCREEN_HEIGHT / 2.0f) * x_max / fmaxf(dist, 0.1f));
				int sprite_width = sprite_height;
				int x = (int)((angle / HALF_FOV + 1.0f) * 0.5f * SCREEN_WIDTH);
				
				SDL_Rect dest = {
					x - sprite_width/2,
					SCREEN_HEIGHT/2 - sprite_height,
					sprite_width,
					sprite_height * 2
				};

				SDL_RenderCopy(renderer, enemy_texture, NULL, &dest);
			
			}
			*/
		}
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
		
		
		
		
		TTF_Font * sans = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
		SDL_Color White = {255,255,255};
		char message[100] = {0};
		snprintf(message, 100, "Hp: %d", hp);
		SDL_Surface * surfaceMessage = TTF_RenderText_Solid(sans, message, White);
		SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);		      
		SDL_Rect destT = {
			100,
			100,
			100, 
    		30 
		};
		SDL_RenderCopy(renderer, Message, NULL, &destT);
		send_player_state(server_socket, player_id,state, hp, p.x, p.y, p.state);
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
    int passed_socket = -1;
    if(argc > 1) {
        passed_socket = atoi(argv[1]);
    }
    net_setup(passed_socket);
    wait_for_game_start();
    TTF_Init();
    player_id = getpid();
    main_loop();
    return 0;
}
