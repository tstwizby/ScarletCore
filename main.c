#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#define DEBUG_VERSION 2
#include "error.c"

/* Debug priorities:
 * Even priorities are reserved for testing purposes, should be relevant at the level below (e.g. 0 for absolute highest priority testing, 2 for mid-level task testing, etc.)
 * 1 - High-level progress, marking what part of the engine is running
 * 3 - Mid-level progress, marking progress within the engine's current task (e.g. loading/building data)
 * 5 - Data loading/building
 * 7 - Data usage (e.g. buffer contents, currently playing song/current location etc.)
 * 9 - Low-level progress, within data loading/processing etc.
 */

//Defines important tools and structs.
#include "buffer.c"

//Defines file path data.
#include "paths.c"

//Defines/enumerates and loads game data.
#include "game.c"

//Defines a number of utility scripts and script-related debug information.
#include "script.c"


//Game Status determines the engine's behavior for each loop:
//  Ready   -	waiting for the player to do something (e.g. open map, examine, etc)
//  Running - 	ready to execute script/write text
//  Waiting - 	waiting for player input to proceed
//  Menu    -	a menu is currently displayed
//  Closed  -	the game has been closed
//  Paused  -	input/updates are paused while something happens (e.g. fadein/out)
//  Map     -	a map is currently displayed
enum GameStatus {READY, RUNNING, WAITING, MENU, CLOSED, PAUSED, MAP};

enum GameStatus status = RUNNING;

//Tracks how long the game has been 'waiting', when set to wait for a certain amount of time.
int delay = 0;


//Contains information for setting up and using SDL, along with graphics/sound information.
#include "SDLWrapper.c"

//Defines the 'TextBox' struct and associated functions.
#include "textbox.c"

//Defines behavior governing textboxes/text display and script information.
#include "text.c"

//Default key bindings.
SDL_Keycode key_select = SDLK_SPACE;
SDL_Keycode key_map = SDLK_c;
SDL_Keycode key_look = SDLK_x;
SDL_Keycode key_act = SDLK_z;
SDL_Keycode key_cancel = SDLK_ESCAPE;
SDL_Keycode key_quit = SDLK_q;

SDL_Keycode key_up = SDLK_UP;
SDL_Keycode key_down = SDLK_DOWN;
SDL_Keycode key_left = SDLK_LEFT;
SDL_Keycode key_right = SDLK_RIGHT;


//Main loop
int SDL_Loop(){
	while(status!=CLOSED){
		//Automatic behavior, that happens independently of the player:
		//If a script/textbox is running and nothing else is blocking it, advance the script/textbox.
		if(status==RUNNING){
			if(countBuffer(txtbox.buffer)){
				tickText(&txtbox);
			}else if(countBuffer(&scrBuffer)){
				scrNext();
			}else{status = READY;}
		}else if(status==PAUSED){
			//Screen is currently fading in/out.
			if(fadeTarget){
				fade();
			}
		}else if(status==WAITING&&delay>0){
			//If delay is 0, waiting for player input instead of counting down.
			if(--delay==0){status = RUNNING;}
		}
		
		//Player-driven behavior
		SDL_Event e;
		while(SDL_PollEvent(&e)){
			int dest = 0;
			SDL_Keycode key;
			switch(e.type){
				case SDL_QUIT:
					status = CLOSED;
					break;
				case SDL_KEYDOWN:
					key = e.key.keysym.sym;
					if(key==SDLK_BACKQUOTE&&status==READY){
						//Hidden copyright notice
						loadCopyright();
					}
					switch(status){
						case RUNNING:
							//Quickly scroll through text
							if(countBuffer(txtbox.buffer)){writeAllText(&txtbox);}
							break;
						case WAITING:
							if(delay==0){status = RUNNING;}
							break;
						case MENU:
							if(key==key_down){
								if(++menuSelect==menuSize){menuSelect=0;}
							}else if(key==key_up){
								if(menuSelect--==0){menuSelect=menuSize-1;}
							}else if(key==key_select){
								selectMenu();
							}else if(key==key_cancel){
								//TODO break out of menu if allowed (currently never applicable)
							}
							break;
						case MAP:
							//NYI
							if(key==key_up){
								
							}else if(key==key_down){
								
							}else if(key==key_select){
								
							}else if(key==key_cancel){
								
							}
							break;
						case READY:
							if(key==key_map){
								//NYI
							}else if(key==key_look){
								clearTextbox(&txtbox);
								loadScript(cLocation.examine);
							}else if(key==key_act){
								clearTextbox(&txtbox);
								loadScript(cLocation.action);
							}else if(key==key_left){
								if(cLocation.l_dist>=0){loadFastTravel(cLocation.left, cLocation.l_dist);}
								else{cLocation.l_arr =  cLocation.left?2:0; clearTextbox(&txtbox); loadScript(cLocation.left);}
							}else if(key==key_right){
								if(cLocation.r_dist>=0){loadFastTravel(cLocation.right,cLocation.r_dist);}
								else{cLocation.r_arr = cLocation.right?2:0; clearTextbox(&txtbox); loadScript(cLocation.right);}
							}else if(key==key_up){
								if(cLocation.u_dist>=0){loadFastTravel(cLocation.up,   cLocation.u_dist);}
								else{cLocation.u_arr =    cLocation.up?2:0; clearTextbox(&txtbox); loadScript(cLocation.up);}
							}else if(key==key_down){
								if(cLocation.d_dist>=0){loadFastTravel(cLocation.down, cLocation.d_dist);}
								else{cLocation.d_arr =  cLocation.down?2:0; clearTextbox(&txtbox); loadScript(cLocation.down);}
							}else if(key==key_cancel){
								status = MENU;
								loadScript(scr_settings);
							}else if(key==key_quit){
								status = CLOSED;
							}
					}
					break;
			}
		}
		renderAll();
		SDL_RenderPresent(renderer);
	}
	return 0;
}


int main(int argc, char* args[]){
	DBG(3,"Loading data...");
	int quitStatus = 0;
	if(initGameValues()){return 1;}
	if(SDLSetup()){return 1;}
	if(quitStatus=fnt_load()){goto font_err;}
	if(quitStatus=loadMedia()){goto media_err;}
	if(quitStatus=scr_load()){goto script_err;}
	if(quitStatus=loc_load()){goto loc_err;}
	setupBuffers();
DBG(3,"Loading startup script");
	loadScript(scr_startup);
DBG(3,"Beginning main loop...");
	quitStatus = SDL_Loop();
DBG(3,"Quitting (Code %d)...",quitStatus);
	loc_err:
	scr_cleanup();
	script_err:
	media_err:
	fnt_cleanup();
	font_err:
	audioCleanup();
	SDL_Cleanup();
	return quitStatus;
}