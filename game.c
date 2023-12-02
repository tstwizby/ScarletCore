//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Defines, ennumerates, and implements game data types.
//Depends on int_types.h, stdio.h, string.h, and paths.c
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Non-Type Data
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Control characters
typedef enum {ctrl_end, ctrl_space, ctrl_line, ctrl_func, ctrl_menu, ctrl_clear, ctrl_wait, ctrl_delim, ctrl_var, ctrl_max=32} CONTROL;
const char* ctrl_names[] = {
	"end","space","line","func","menu","clear","wait","delim","var"
};

//Resolutions
typedef enum{res_1440p,res_1080p,res_720p,res_480p,res_count} RESOLUTION;
const char* res_names[] = {
	"1440p (2560x1440)",
	"1080p (1920x1080)",
	 "720p (1280x720)",
	 "480p (640x480)",
};

//Struct describes layout of UI elements at the given resolution.
typedef struct _Resolution{
	RESOLUTION index;
	int screen_width;
	int screen_height;
	int bg_width;
	int bg_height;
	int bg_x;
	int bg_y;
	int arrows_x;
	int arrows_y;
	int txt_width;
	int txt_height;
	int txt_x;
	int txt_y;
	int menu_width;
	int menu_x;
	int menu_y;
} Resolution;

const Resolution res_1440p_s = {
			.index = res_1440p,
	 .screen_width = 2560,
	.screen_height = 1440,
		 .bg_width = 1920,
		.bg_height = 1080,
			 .bg_x =  320,
			 .bg_y =   80,
		 .arrows_x =  343,
		 .arrows_y = 1183,
		.txt_width = 1744,
	   .txt_height =  176,
			.txt_x =  496,
			.txt_y = 1160,
	   .menu_width =  840,
		   .menu_x =  860,
		   .menu_y =  100};
const Resolution res_1080p_s = {
			.index = res_1080p,
	 .screen_width = 1920,
	.screen_height = 1080,
		 .bg_width = 1440,
		.bg_height =  810,
			 .bg_x =  240,
			 .bg_y =   40,
		 .arrows_x =  263,
		 .arrows_y =  873,
		.txt_width = 1264,
	   .txt_height =  176,
			.txt_x =  416,
			.txt_y =  850,
	   .menu_width =  840,
		   .menu_x =  540,
		   .menu_y =   60};
const Resolution res_720p_s = {
			.index = res_720p,
	 .screen_width = 1280,
	.screen_height =  720,
		 .bg_width =  960,
		.bg_height =  540,
			 .bg_x =  160,
			 .bg_y =   30,
		 .arrows_x =  170,
		 .arrows_y =  577,
		.txt_width =  810,
	   .txt_height =  144,
			.txt_x =  310,
			.txt_y =  570,
	   .menu_width =  640,
		   .menu_x =  320,
		   .menu_y =   50};
const Resolution res_480p_s = {
			.index = res_480p,
	 .screen_width = 640,
	.screen_height = 480,
		 .bg_width = 480,
		.bg_height = 270,
			 .bg_x = 140,
			 .bg_y =  20,
		 .arrows_x =   5,
		 .arrows_y = 150,
		.txt_width = 600,
	   .txt_height = 164,
			.txt_x =  20,
			.txt_y = 300,
	   .menu_width = 480,
		   .menu_x =  80,
		   .menu_y =  40};
Resolution resolutions[res_count];

//Initializes the resolutions array for convenient access.
int res_init(){
	resolutions[0] = res_1440p_s;
	resolutions[1] = res_1080p_s;
	resolutions[2] =  res_720p_s;
	resolutions[3] =  res_480p_s;
}
#define res_default res_720p
Resolution* res;


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Types
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef enum {type_func, type_smac, type_clr, type_wth, type_dir, type_crd, type_sky, type_shd, type_lde, type_cfg, type_vop, type_chk, type_scr, type_loc, type_mus, type_sef, type_scn, type_var, type_fnt, type_num, type_count} PARAM_TYPE;
const char* type_names[] = {
	"function",				//A function
	"macro",				//A macro consisting of a sequence of functions
	"color",				//Used for rendering colors, fade out/in, etc.
	"weather",				//Used as game information and to draw proper backgrounds
	"direction",			//A direction relative to the current view
	"cardinal direction",	//An absolute direction
	"sky type",				//Describes to what degree the sky is visible in a given location
	"shade type",			//Describes to what degree the time of day affects a location's appearance
	"location data element",//A static characteristic of a location
	"config variable",		//A game setting set in the config menu
	"variable modifier",	//Describes a way to modify a numeric variable
	"variable check",		//Describes a way to compare two numeric variables
	"script",				//A loaded script
	"location",				//A location
	"music",				//A piece of background music
	"sound",				//A sound effect
	"scene",				//A cutscene image
	"game variable",		//A registered variable to measure game progress
	"font",					//A font
	"number",				//A number
};

//Types all have a three-letter abbreviation (except functions/macros, which use four letters), a name, and an index as assigned by the enumeration.
//Type data always follows the format [abbr]_[data type], for example 'scr_names'.


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Functions
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef enum {	f_null, f_quit, f_stop, f_goto, f_run, f_runback, f_fadeout, f_fadein, f_transition, f_playBGM, f_playSE, f_silence, f_addtime, 
				f_setcfg, f_setvar, f_checkvar, f_switchvar, f_setlocscript, f_setlocdest, f_wait, f_setscene,
				func_count} FUNCTION;
char* func_names[] = {
	"null",			//No-op
	"quit",			//Closes the game
	"stop",			//Exits the current script
	"goto",			//Exits the current script and passes execution to another
	"run",			//Passes execution to another script, and loops back to this one when it finishes
	"runback",		//Passes execution to another script, returns to the beginning of this one when it finishes
	"fadeout",		//Fadeout(color, speed)
	"fadein",		//Same as fadeout
	"transition",	//Changes the background
	"playBGM",		//Changes the background music
	"playSE",		//Plays a sound effect
	"silence",		//Turns off the background music, either abruptly with an argument of 0 or fading out with some delay
	"addtime",		//Adds an amount of time in hours/minutes
	"setcfg",		//Sets a config variable to a given value
	"setvar",		//Changes a game variable in the described way
	"checkvar",		//Checks to see if the game matches a given game state, runs the given script if so, and continues if not
	"switchvar",	//If the game matches a given game state, exits current script and passes control to another, and continues if not
	"setlocscript", //Overrides a location script for a given location
	"setlocdest",   //Overrides a movement destination for a given location
	"wait",			//Does nothing for a certain amount of time
	"setscene",		//Shows a scene
};

//Number of arguments for each function
const int func_arg_count[] = {
	0,0,0,1,1,1,2,1,1,3,3,1,2,2,3,4,4,4,4,1,1
};
//Types of each argument
const int* func_arg_types[] = {
	(int[]){},
	(int[]){},
	(int[]){},
	(int[]){type_scr},
	(int[]){type_scr},
	(int[]){type_scr},
	(int[]){type_clr,	type_num},
	(int[]){type_num},
	(int[]){type_loc},
	(int[]){type_mus,	type_num,	type_num},
	(int[]){type_sef,	type_num,	type_num},
	(int[]){type_num},
	(int[]){type_num,	type_num},
	(int[]){type_cfg,	type_num},
	(int[]){type_var,	type_vop,	type_num},
	(int[]){type_var,	type_chk,	type_num,	type_scr},
	(int[]){type_var,	type_chk,	type_num,	type_scr},
	(int[]){type_loc,	type_lde,	type_scr,	type_num},
	(int[]){type_loc,	type_dir,	type_loc,	type_num},
	(int[]){type_num},
	(int[]){type_scn},
};
//Names of each argument (used for debugging)
char** func_arg_names[] = {
	(char*[]){},
	(char*[]){},
	(char*[]){},
	(char*[]){"new"},
	(char*[]){"sub"},
	(char*[]){"sub"},
	(char*[]){"fade","time"},
	(char*[]){"time"},
	(char*[]){"destination"},
	(char*[]){"song","fadein","loops"},
	(char*[]){"effect","volume","speed"},
	(char*[]){"fadeout"},
	(char*[]){"hours","minutes"},
	(char*[]){"var","value"},
	(char*[]){"var","operation","value"},
	(char*[]){"var","check","value","sub"},
	(char*[]){"var","check","value","new"},
	(char*[]){"target","element","new","arrow"},
	(char*[]){"target","exit","destination","distance"},
	(char*[]){"time"},
	(char*[]){"target"},
};


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Macros
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef enum{smac_moveto,smac_moveandplay,smac_fadeall,smac_fadeinscene,smac_fadeoutscene,smac_count} S_MACRO;
char* smac_names[] = {
	"moveto",
	"moveandplay",
	"fadeall",
	"showscene",
	"clearscene",
};

//Number of arguments for each macro
const int smac_arg_count[] = {
	4,5,2,3,2
};
//Types of each argument
const int* smac_arg_types[] = {
	(int[]){type_loc,type_clr,type_num,type_sef},
	(int[]){type_loc,type_clr,type_num,type_sef,type_mus},
	(int[]){type_clr,type_num},
	(int[]){type_scn,type_clr,type_num},
	(int[]){type_num,type_clr},
};
//Names of each argument (used for debugging)
char** smac_arg_names[] = {
	(char*[]){"destination","fade","time","sound"},
	(char*[]){"destination","fade","time","sound","music"},
	(char*[]){"fade","time"},
	(char*[]){"target","fade","time"},
	(char*[]){"time","fade"},
};
//Length of each macro
const int smac_size[] = {
	11,15,5,7,7,
};
//Expansion of each macro
const int* smac_expand[] = {
	(int[]){f_fadeout,-2,-3,f_transition,-1,f_playSE,-4,100,0,f_fadein,-3},
	(int[]){f_fadeout,-2,-3,f_transition,-1,f_playSE,-4,100,0,f_playBGM,-5,-3,0,f_fadein,-3},
	(int[]){f_fadeout,-1,-2,f_silence,-2},
	(int[]){f_fadeout,-2,-3,f_setscene,-1,f_fadein,-3},
	(int[]){f_fadeout,-2,-1,f_setscene,0,f_fadein,-1},
};


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Enumerated Types
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Colors- Used for rendering colors, fade out/in, etc.
typedef enum {clr_black, clr_white, clr_count} COLOR;
char* clr_names[] = {
	"black",
	"white"
};
//RGBA codes
const int colormap[][4] = {
	{0x00, 0x00, 0x00, 0xFF},
	{0xFF, 0xFF, 0xFF, 0xFF}
};


//Weather- Used as game information and to draw proper backgrounds
typedef enum {wth_clear, wth_cloud, wth_storm, wth_mist, wth_count} WEATHER;
char* wth_names[] = {
	"clear",
	"cloud",
	"storm",
	"mist"
};


//Direction- A direction relative to the current view
typedef enum {dir_up, dir_down, dir_left, dir_right, dir_any, dir_count} DIRECTION;
char* dir_names[] = {"up","down","left","right","any"};


//Cardinal Direction- An absolute direction
typedef enum {crd_north, crd_south, crd_east, crd_west, crd_any, crd_count} CARDINAl;
char* crd_names[] = {"N","S","E","W","A"};


//Sky Type- Describes to what degree the sky is visible in a given location
typedef enum {	sky_none,	//No sky visible, e.g. indoor locations without windows
				sky_full,	//Sky always visible
				sky_limited,//Sky mostly visible, but evening sky becomes night sky
				sky_count
} SKY_TYPE;
char* sky_names[] = {"none","full","limited"};


//Shade Type- Describes to what degree the time of day affects a location's appearance
typedef enum {	shd_none,		//No shade at any time
				shd_full,		//Day, evening, and night shading
				shd_limited,	//Day and night shading, the latter used both at night and in the evening.
				shd_count
} SHADE_TYPE;
char* shd_names[] = {"none","full","limited"};


//Location Data Element- A static characteristic of a location
typedef enum {lde_enter, lde_act, lde_ex, lde_up, lde_down, lde_left, lde_right, lde_count} LDE;
char* lde_names[] = {"enter","act","ex","up","down","left","right"};


//Config Variable- A game setting set in the config menu
#define DEFAULT_TEXT_SPEED 2
#define DEFAULT_VOLUME   100
#define DEFAULT_RESOLUTION 3
typedef enum{cfg_text_speed,cfg_sef_volume,cfg_mus_volume,cfg_resolution,cfg_count} CONFIG_VAR;
int cfgVars[cfg_count];
const int cfgDefault[cfg_count] = {
	DEFAULT_TEXT_SPEED,DEFAULT_VOLUME,DEFAULT_VOLUME,res_default
};
char* cfg_names[cfg_count] = {
	"text_speed","sound_volume","music_volume","resolution"
};

#define text_speed cfgVars[cfg_text_speed]
#define mus_volume cfgVars[cfg_mus_volume]
#define sef_volume cfgVars[cfg_sef_volume]
#define resolution cfgVars[cfg_resolution]

//Sets config variables to their default values
int cfg_init(){
	for(int i=0;i<cfg_count;i++){cfgVars[i] = cfgDefault[i];}
}


//Variable Modifier- Describes a way to modify a numeric variable
typedef enum{vop_set,vop_add,vop_sub,vop_count} VAR_OP;
char* vop_names[] = {"set","add","subtract"};

#define MAKE_VOP(vop,statement) int vop##_func(uint8_t var,uint8_t x){return statement;}
MAKE_VOP(vop_set,x)
MAKE_VOP(vop_add,var+x)
MAKE_VOP(vop_sub,var+x)
int (*vop_funcs[vop_count])(uint8_t,uint8_t) = {vop_set_func,vop_add_func,vop_sub_func};


//Variable Check- Describes a way to compare two numeric variables
typedef enum{chk_eq,chk_lo,chk_hi,chk_count} VAR_CHECK;
char* chk_names[] = {"equals","lower","higher"};

#define MAKE_CHK(chk,statement) int chk##_func(uint8_t var,uint8_t x){return statement;}
MAKE_CHK(chk_eq,var==x)
MAKE_CHK(chk_lo,var<x)
MAKE_CHK(chk_hi,var>x)
int(*chk_funcs[chk_count])(uint8_t,uint8_t) = {chk_eq_func,chk_lo_func,chk_hi_func};


//Initialize list of type counts with enumerated type counts
int all_count[type_count] = {
	func_count,
	smac_count,
	 clr_count,
	 wth_count,
	 dir_count,
	 crd_count,
	 sky_count, 	
	 shd_count,
	 lde_count,
	 cfg_count,
	 vop_count,
	 chk_count,
};


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Non-Enumerated Types
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Non-enumerated types are loaded from an external file, and have a maximum number that can be registered at a time.
//They can have required 'set' members, which much exist (with a particular name) for the system to run, for example a 'startup' script or particular data featured in a macro.
//These types all have functions (*only if applicable to the type in question):
//			 [type]_init     - Sets the data for the given type to default values
//			 [type]_register - Registers a name for use by the type
//			*[type]_validate - Ensures that all set members of the type are present
//			*[type]_cleanup  - Frees any memory allocated for the type
//			 [type]_load     - Loads the data for the type from the relevant data file


//Generic registration function, used as a base for [type]_register functions.
//Validates unique names within each type and limits on numbers of registered members of each type.
#define registerThing(A, B)	if(A##_count==MAX_##B##_COUNT){ERR(1,"Max number of %ss registered",type_names[type_##A]);}\
							for(int i=0;i<A##_count;i++){\
								if(!strcmp(A##_names[i],name)){ERR(1,"Duplicate %s registered (%s)",type_names[type_##A],name);}\
							}\
							int len = strlen(name);\
							if(len>=MAX_##B##_NAME_LEN){ERR(1,"'%s' exceeds %s name length limits",name,type_names[type_##A]);}\
							memcpy(A##_names[A##_count],name,len+1);\
							DBG(5,"Registered %s '%s'",type_names[type_##A],name); A##_count++

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Script
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MAX_SCR_COUNT 256
#define MAX_SCR_NAME_LEN 16
#define scr_count all_count[type_scr]
char  scr_names[MAX_SCR_COUNT][MAX_SCR_NAME_LEN];
char* scr_names_lnk[MAX_SCR_COUNT];

//Required scripts
typedef enum{scr_set_startup,scr_set_settings,scr_set_evening,scr_set_night,scr_set_count}SCR_SET;
const char* scr_set_names[] = {
	"startup",
	"settings",
	"evening",
	"night"
};
int scr_set[scr_set_count];

#define scr_startup  scr_set[scr_set_startup]
#define scr_settings scr_set[scr_set_settings]
#define scr_evening  scr_set[scr_set_evening]
#define scr_night    scr_set[scr_set_night]

#define SCRIPT_MAX_LENGTH 4096 //65535?
uint8_t* scr_buffers[MAX_SCR_COUNT];
int 	 scr_len[MAX_SCR_COUNT];

int scr_init(){
	//Script 0 is always a default no-op script.
	scr_buffers[0] = malloc(1);			//Freed in scr_cleanup
	if(scr_buffers[0]==NULL){ERR(2,"Could not initialize default script.");}
	scr_buffers[0][0] = ctrl_end;
	scr_len[0] = 1;
	//Clear unloaded script data
	for(int i=1;i<MAX_SCR_COUNT;i++){
		scr_buffers[i] = NULL;
		scr_len[i] = 0;
	}
	//Set scripts not yet found
	for(int i=0;i<scr_set_count;i++){
		scr_set[i] = -1;
	}
	return 0;
}

int scr_register(const char* name){
	registerThing(scr,SCR);
	for(int i=0;i<scr_set_count;i++){
		//Check if set script
		if(!strcmp(scr_set_names[i],name)){
			scr_set[i] = scr_count-1;
			DBG(5,"Set script '%s' found.",scr_set_names[i]);
		}
	}
	return 0;
}

//Validate that all required scripts are present
int scr_validate(){
	for(int i=0;i<scr_set_count;i++){
		if(scr_set[i]<0){ERR(1,"Missing required script \"%s\".",scr_set_names[i]);}
	}
	return 0;
}

int scr_cleanup(){
	for(int i=0;i<scr_count;i++){
		free(scr_buffers[i]);
	}
}

int scr_load(){
	DBG(3,"Loading scripts...");
	FILE* source = fopen(script_bin,"rb");
	if(source == NULL){ERR(1,"Could not open script source file '%s'.",script_bin);}
	for(int sc=1;sc<scr_count;sc++){
		DBG(5,"Loading script %s...",scr_names[sc]);
		int len = readInt(source);
		if(len<0){fclose(source);scr_cleanup();ERR(2,"Could not read script length.");}
		scr_buffers[sc] = malloc(len+1);	//Freed in scr_cleanup()
		if(scr_buffers[sc]==NULL){scr_cleanup();ERR(3,"Problem allocating memory for script.");}
		if(fread(scr_buffers[sc],1,len,source)<len){fclose(source);scr_cleanup();ERR(2,"Could not read script data.");}
		scr_len[sc]=len+1;
		scr_buffers[sc][len]=ctrl_end;
	}
	DBG(3,"Scripts loaded.");
	return 0;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Location
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MAX_LOC_COUNT 64
#define MAX_LOC_NAME_LEN 16
#define loc_count all_count[type_loc]
char  loc_names[MAX_LOC_COUNT][MAX_LOC_NAME_LEN];
char* loc_names_lnk[MAX_LOC_COUNT];
int   loc_name_lengths[MAX_LOC_COUNT];
char  loc_paths[MAX_LOC_COUNT][MAX_LOC_NAME_LEN+16];

typedef struct _Location{
	int index;
	char* name;
	DIRECTION dir;
	SKY_TYPE sky;
	SHADE_TYPE shade;
	int left;			//Index of location or script to the left of this location (similar for right/up/down)
	int right;
	int up;
	int down;
	int l_dist;			//Distance to location to the left of this location, or if negative, indicator that index is for a script rather than location (similar for right/up/down)
	int r_dist;
	int u_dist;
	int d_dist;
	int l_arr;			//Status of the displayed up arrow at this location (0- empty, 1- active, 2- inactive) (similar for right/up/down)
	int r_arr;
	int u_arr;
	int d_arr;
	int script;
	int action;
	int examine;
} Location;

Location locations[MAX_LOC_COUNT];
#define cLocation locations[gameVars[var_location]]

int loc_init(int i){
	if(0>i||i>=MAX_LOC_COUNT){ERR(1,"Location index out of bounds");}
	locations[i].index  =  i;
	locations[i].name   =  loc_names[i];
	locations[i].dir    =  0;
	locations[i].sky    =  0;
	locations[i].shade  =  0;
	locations[i].left   = -1;
	locations[i].l_dist =  0;
	locations[i].l_arr  =  0;
	locations[i].right  = -1;
	locations[i].r_dist =  0;
	locations[i].r_arr  =  0;
	locations[i].up     = -1;
	locations[i].u_dist =  0;
	locations[i].u_arr  =  0;
	locations[i].down   = -1;
	locations[i].d_dist =  0;
	locations[i].d_arr  =  0;
	locations[i].script = -1;
	locations[i].action = -1;
	locations[i].examine= -1;
	return 0;
}
int loc_build(Location* loc, int index, DIRECTION dir, SKY_TYPE sky, SHADE_TYPE shade){
	if(0>index||index>=loc_count){ERR(1,"Location index out of bounds");}
	loc->index  =  index;
	loc->name   =  loc_names[index];
	loc->dir    =  dir;
	loc->sky    =  sky;
	loc->shade  =  shade;
	loc->left   = -1;
	loc->l_dist =  0;
	loc->l_arr  =  0;
	loc->right  = -1;
	loc->r_dist =  0;
	loc->r_arr  =  0;
	loc->up     = -1;
	loc->u_dist =  0;
	loc->u_arr  =  0;
	loc->down   = -1;
	loc->d_dist =  0;
	loc->d_arr  =  0;
	loc->script = -1;
	loc->action = -1;
	loc->examine= -1;
	return 0;
}

int loc_register(const char* name){
	registerThing(loc,LOC);
	loc_name_lengths[loc_count-1] = len;
#ifndef COMPILER
	buildPath(loc,loc_count-1);
	return loc_init(loc_count-1);
#else
	return 0;
#endif
}

int loc_load(){
	DBG(3,"Building locations...");
	FILE* source = fopen(loc_bin,"rb");
	if(source == NULL){ERR(1,"Could not open location source file '%s'.",loc_bin);}
	for(int lc=0;lc<loc_count;lc++){
		DBG(5,"Loading location %s...",loc_names[lc]);
		uint8_t loc_buffer[18];
		if(fread(loc_buffer,18,1,source)<1){fclose(source);ERR(2,"Failed to read data for location %s.",loc_names[lc]);}
		//Bytes 0-2 of the buffer contain the direction, sky, and shade data.
		locations[lc].dir     = loc_buffer[0];
		locations[lc].sky     = loc_buffer[1];
		locations[lc].shade   = loc_buffer[2];
		//Bytes 3-10 contain the directional data, and byte 11 encodes whether the directional data is for scripts or other locations.
		locations[lc].u_dist  =(loc_buffer[11]&8)?loc_buffer[4]:-1;
		locations[lc].d_dist  =(loc_buffer[11]&4)?loc_buffer[6]:-1;
		locations[lc].l_dist  =(loc_buffer[11]&2)?loc_buffer[8]:-1;
		locations[lc].r_dist  =(loc_buffer[11]&1)?loc_buffer[10]:-1;
		locations[lc].u_arr   = loc_buffer[3]?1:0;
		locations[lc].d_arr   = loc_buffer[5]?1:0;
		locations[lc].l_arr   = loc_buffer[7]?1:0;
		locations[lc].r_arr   = loc_buffer[9]?1:0;
		locations[lc].up      = loc_buffer[3];
		locations[lc].down    = loc_buffer[5];
		locations[lc].left    = loc_buffer[7];
		locations[lc].right   = loc_buffer[9];
		//Bytes 13-17 encode the scripts associated with the location.
		locations[lc].script  = loc_buffer[13]+loc_buffer[12]*256;
		locations[lc].action  = loc_buffer[15]+loc_buffer[14]*256;
		locations[lc].examine = loc_buffer[17]+loc_buffer[16]*256;
	}
	DBG(3,"Locations built.");
	return 0;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Music
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MAX_MUS_COUNT 32
#define MAX_MUS_NAME_LEN 16
#define mus_count all_count[type_mus]
char  mus_names[MAX_MUS_COUNT][MAX_MUS_NAME_LEN];
char* mus_names_lnk[MAX_MUS_COUNT];
int   mus_name_lengths[MAX_MUS_COUNT];
char  mus_paths[MAX_MUS_COUNT][MAX_MUS_NAME_LEN+12];
//TODO better music looping, intros, etc.

int mus_register(const char* name){
	registerThing(mus,MUS);
	mus_name_lengths[mus_count-1] = len;
#ifndef COMPILER
	buildPath(mus,mus_count-1);
#endif
	return 0;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Sound
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MAX_SEF_COUNT 128
#define MAX_SEF_NAME_LEN 16
#define sef_count all_count[type_sef]
char  sef_names[MAX_SEF_COUNT][MAX_SEF_NAME_LEN];
char* sef_names_lnk[MAX_SEF_COUNT];
int   sef_name_lengths[MAX_SEF_COUNT];
 
char sef_paths[MAX_SEF_COUNT][MAX_SEF_NAME_LEN+11];

int sef_steps = 0;

int sef_register(const char* name){
	registerThing(sef,SEF);
	sef_name_lengths[sef_count-1] = len;
#ifndef COMPILER
//TODO- need better process for this, maybe make into a macro? Need to figure out how that would work.
	if(!strcmp(name,"steps")){
		sef_steps = sef_count-1;
		DBG(5,"'steps' sound effect found.");
	}
	buildPath(sef,sef_count-1);
#endif
	return 0;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Scene
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MAX_SCN_COUNT 32
#define MAX_SCN_NAME_LEN 16
#define scn_count all_count[type_scn]
char  scn_names[MAX_SCN_COUNT][MAX_SCN_NAME_LEN];
char* scn_names_lnk[MAX_SCN_COUNT];
int   scn_name_lengths[MAX_SCN_COUNT];

char scn_paths[MAX_SCN_COUNT][MAX_SCN_NAME_LEN+11];

//Currently displayed scene, if any.
int scene = 0;

int scn_register(const char* name){
	registerThing(scn,SCN);
	scn_name_lengths[scn_count-1] = len;
#ifndef COMPILER
	buildPath(scn,scn_count-1);
#endif
	return 0;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Game Variable
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MAX_VAR_COUNT 32
#define MAX_VAR_NAME_LEN 16

typedef enum{var_location,var_weather,var_time_hr,var_time_min,var_music,var_setCount} setVars;

#define location gameVars[var_location]
#define weather gameVars[var_weather]
#define hours gameVars[var_time_hr]
#define minutes gameVars[var_time_min]
#define bgm gameVars[var_music]

#define var_count all_count[type_var]

uint8_t gameVars[MAX_VAR_COUNT];
char    var_names[MAX_VAR_COUNT][MAX_VAR_NAME_LEN];
char*   var_names_lnk[MAX_VAR_COUNT];

int var_register(const char* name){
	registerThing(var,VAR);
	return 0;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Fonts
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Some notes on font format:
//  fonts are stored at the moment in .dat files in /fonts, along with png textures
//  the font data format consists of an eight byte header followed by character width data
//  the header contains the following data:
//    1- the height of the font in pixels
//    2- the maximum character width in pixels
//    3- the number of characters in a row in the image
//    4- the number of rows containing characters in the image
//    5- the vertical offset of each row (to account for letters like 'g' or 'q' that descend below the bottom of a line)
//    6- the width of a space
//    7- dummy
//    8- dummy

#define MAX_FNT_COUNT 1
#define MAX_FNT_NAME_LEN 16
#define fnt_count all_count[type_fnt]
char  fnt_names[MAX_FNT_COUNT][MAX_FNT_NAME_LEN];
char* fnt_names_lnk[MAX_FNT_COUNT];
int   fnt_name_lengths[MAX_FNT_COUNT];

char fnt_paths[MAX_FNT_COUNT][MAX_FNT_NAME_LEN+10];
char fnt_data_paths[MAX_FNT_COUNT][MAX_FNT_NAME_LEN+10];

//Font struct carries info about the dimensions of the font's characters.
typedef struct _Font{
	int height;
	int width;
	int columns;
	int rows;
	int voffset;
	int space;
	uint8_t* charwidth;
} Font;

Font fonts[MAX_FNT_COUNT];

int fnt_init(int i){
	if(0>i||i>=MAX_FNT_COUNT){ERR(1,"Font index out of bounds");}
	fonts[i].height   = 0;
	fonts[i].width    = 0;
	fonts[i].columns  = 0;
	fonts[i].rows     = 0;
	fonts[i].voffset  = 0;
	fonts[i].space    = 0;
	fonts[i].charwidth= 0;
	return 0;
}

int fnt_register(const char* name){
	registerThing(fnt,FNT);
	fnt_name_lengths[fnt_count-1] = len;
#ifndef COMPILER
	buildPath(fnt,fnt_count-1);
	buildDataPath(fnt,fnt_count-1);
	return fnt_init(fnt_count-1);
#else
	return 0;
#endif
}

int fnt_load(){
	DBG(3,"Loading fonts...");
	for(int i=0;i<fnt_count;i++){
		DBG(5,"\tLoading font %s...",fnt_names[i]);
		FILE *fnt_source;
		uint8_t header[8];
		fnt_source = fopen(fnt_data_paths[i], "rb");
		if(fread(header,1,8,fnt_source)<8){fclose(fnt_source);ERR(2,"Error reading header of font data file %s",fnt_data_paths[i]);}
		fonts[i].height	= header[0];
		fonts[i].width	= header[1];
		fonts[i].columns= header[2];
		fonts[i].rows	= header[3];
		fonts[i].voffset= header[4];
		fonts[i].space	= header[5];
		int ccount = header[2]*header[3];
		fonts[i].charwidth = malloc(ccount);	//Freed in fnt_cleanup
		if(fread(fonts[i].charwidth, 1, ccount, fnt_source)<ccount){fclose(fnt_source);ERR(2,"Error reading font data file %s",fnt_data_paths[i]);}
		fclose(fnt_source);
	}
	DBG(3,"Fonts loaded.");
	return 0;
}

int fnt_cleanup(){
	for(int i=0;i<fnt_count;i++){free(fonts[i].charwidth);}
	return 0;
}
//Calculates the length of a word in the given font in pixels, used to determine line wrapping. Fails for words longer than 100 characters.
int wordLength(int f, uint8_t* data){
	int len = 0;
	for(int i=0;data[i]>ctrl_max&&i<100;i++){
		len += fonts[f].charwidth[data[i]-ctrl_max-1];
	}
	return len;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Numeric Types
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int num_parse(int* num, char* name){
	char* rem;
	*num = strtol(name,&rem,10);
	if((*num==0&&name[0]!='0')||rem[0]){return 0;}
	return 1;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Debug/Utility functions
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

char** all_names[type_count] = {
	func_names,
	smac_names,
	clr_names,
	wth_names,
	dir_names,
	crd_names,
	sky_names,
	shd_names,
	lde_names,
	cfg_names,
	vop_names,
	chk_names,
	scr_names_lnk,
	loc_names_lnk,
	mus_names_lnk,
	sef_names_lnk,
	scn_names_lnk,
	var_names_lnk,
	NULL,			//Numbers don't have names
};


//Searches for a given name of the given type, returns its index (or itself, if a numeric type) if found or -1 otherwise.
int searchNames(int type, char* name){
	if(type<0||type>=type_count){ERR(-1,"Unrecognized type");}
	else if(type==type_num){
		int res;
		if(num_parse(&res,name)){return res;}
		return -1;
	}else{
		for(int i=0;i<all_count[type];i++){
			if(!strcmp(all_names[type][i],name)){return i;}
		}
		return -1;
	}
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Time-related data
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Time of day
typedef enum {tod_day, tod_evening, tod_night, tod_count} TIME_OF_DAY;
const char* tod_names[] = {
	"day",
	"evening",
	"night"
};

//Special game-relevant times
#define SHIFT_START 6
#define EVENING_START 8
#define NIGHT_START 10
#define SHIFT_END 15

//Returns day/evening/night based on the current time
TIME_OF_DAY getTimeOfDay(){
	if(hours>=EVENING_START){
		if(hours>=NIGHT_START){return tod_night;}
		return tod_evening;
	}
	return tod_day;
}

int addTime(int h, int m){
	TIME_OF_DAY tod = getTimeOfDay();
	hours   += h;
	minutes += m;
	if(minutes>=60){minutes -= 60;hours++;}
	if(scr_evening&&tod==tod_day&&getTimeOfDay()==tod_evening){return scr_evening;}
	if(scr_night&&tod==tod_evening&&getTimeOfDay()==tod_night){return scr_night;}
	return 0;
}
int setTime(int h, int m){
	hours   = h;
	minutes = m;
}


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Tools for finding the appropriate backgrounds for the sky/shade
//Really hacked together for now, will be a lot cleaner once I've actually drawn all the backgrounds and figured out what to cut out.
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const char* wth_paths[] = {
	NULL,
	SKY_PATH(A_day_clear),	// 1
	SKY_PATH(A_day_cloud),	// 2
	SKY_PATH(A_day_storm),	// 3
	SKY_PATH(A_ngt_clear),	// 4
	SKY_PATH(A_ngt_cloud),	// 5
	SKY_PATH(A_ngt_storm),	// 6
	SKY_PATH(N_eve_clear),	// 7
	SKY_PATH(N_eve_cloud),	// 8
	SKY_PATH(W_eve_clear),	// 9
	SKY_PATH(W_eve_cloud)	//10
};

const int sky_map[sky_count][tod_count][wth_count][dir_any] = {
	//sky_type_none
	{{{0},{0},{0},{0}},
	 {{0},{0},{0},{0}},
	 {{0},{0},{0},{0}}},
	//sky_type_full
	{{{ 1, 1, 1, 1},{ 2, 2, 2, 2},{ 3, 3, 3, 3},{ 3, 3, 3, 3}},
	 {{ 7, 0, 0, 9},{ 8, 0, 0,10},{ 6, 6, 6, 6},{ 6, 6, 6, 6}},
	 {{ 4, 4, 4, 4},{ 5, 5, 5, 5},{ 6, 6, 6, 6},{ 6, 6, 6, 6}}},
	//sky_type_limited
	{{{ 1, 1, 1, 1},{ 2, 2, 2, 2},{ 3, 3, 3, 3},{ 3, 3, 3, 3}},
	 {{ 4, 4, 4, 4},{ 5, 5, 5, 5},{ 6, 6, 6, 6},{ 6, 6, 6, 6}},
	 {{ 4, 4, 4, 4},{ 5, 5, 5, 5},{ 6, 6, 6, 6},{ 6, 6, 6, 6}}}
};
//Generates the path to the appropriate sky background for the current location, weather, and time of day.
const char* getSkyPath(){
	return wth_paths[sky_map[cLocation.sky][getTimeOfDay()][weather][cLocation.dir]];
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Game variables and data
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MAX_NAME_LEN 64
//Loads data of a particular type 'A', setting aside 'O' preset values.
#define LOAD_GAME_DAT(O,A) \
DBG(3,"Loading %s names...",type_names[type_##A]);\
count = readInt(data);\
if(count<0){ERR(2,"Problem reading %s count",type_names[type_##A]);}\
for(int i=O;i<count;i++){\
	int nlen=fgetc(data);\
	if(nlen<0){ERR(2,"Problem reading name length");}\
	if(nlen>MAX_NAME_LEN){ERR(1,"Generic name length restrictions exceeded (%d)",nlen);}\
	fread(name,1,nlen,data);\
	name[nlen]=0;\
	if(A##_register(name)){return 1;}\
}DBG(3,"Done")


int loadGameValues(FILE* data){
	int count = 0;
	char name[MAX_NAME_LEN];
	LOAD_GAME_DAT(1,scr);
	LOAD_GAME_DAT(0,loc);
	LOAD_GAME_DAT(1,mus);
	LOAD_GAME_DAT(1,sef);
	LOAD_GAME_DAT(1,scn);
	LOAD_GAME_DAT(5,var);
	return 0;
}

int cfg_load(FILE* data){
	char buffer[2048];
	char* rem;
	for(int i=0;i<cfg_count;i++){
		//Config variable description
		if(!fgets(buffer,2048,data)){ERR(1,"Could not read config file.");}
		//Config variable value
		if(!fgets(buffer,2048,data)){ERR(1,"Could not read config file.");}
		int cval = strtol(buffer,&rem,10);
		if(cval==0&&buffer[0]!='0'){
			DBG(1,"Could not read %s from config file (%s), setting to default.",cfg_names[i],buffer);
			cfgVars[i] = cfgDefault[i];
		}
		cfgVars[i] = cval;
	}
}

//Setting initial values for important game constants
#define linkNames(A,B) for(int i=0;i<MAX_##B##_COUNT;i++){A##_names_lnk[i]=&(A##_names[i][0]);}
int initGameValues(){
	linkNames(scr,SCR);
	linkNames(loc,LOC);
	linkNames(mus,MUS);
	linkNames(sef,SEF);
	linkNames(scn,SCN);
	linkNames(var,VAR);
	res_init();
	DBG(3,"Registering initial data");
	scr_register("none");
	scn_register("none");
	sef_register("none");
	mus_register("silence");
	var_register("location");
	var_register("weather");
	var_register("hour");
	var_register("minute");
	var_register("music");
	fnt_register("testfont");
	weather = wth_clear;
	hours   = SHIFT_START;
	minutes = 0;
	bgm     = 0;
	#ifndef COMPILER
	DBG(3,"Loading config data from '%s'...",cfg_source);
	FILE* data = fopen(cfg_source,"r");
	if(data==NULL){ERR(1,"Could not open configuration file '%s'",cfg_source);}
	cfg_load(data);
	res = resolutions+cfgVars[cfg_resolution];
	DBG(3,"Loading data from '%s'...",dat_bin);
	data = fopen(dat_bin,"rb");
	if(data==NULL){ERR(1,"Could not open file");}
	int err = loadGameValues(data);
	fclose(data);
	return err;
	#endif
	cfg_init();
	return 0;
}