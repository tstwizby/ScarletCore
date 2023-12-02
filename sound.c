//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Wrapper for SDL_mixer to separate audio logic from game logic.
//Meant to be included by SDLWrapper
//Depends on game.c
//TODO: have intro/loop split for music, track sound effect play time
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <SDL_mixer.h>
//MAX_MUS_COUNT and MAX_SEF_COUNT from game.c
Mix_Music* music[MAX_MUS_COUNT];
Mix_Chunk* soundEffects[MAX_SEF_COUNT];	

int resetAudio(){
	Mix_VolumeMusic(mus_volume);
}

int audioSetup(){
	DBG(3,"Setting up audio...");
	if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048)<0){ERR(1,"Failed to initialize SDL_mixer");}
	Mix_VolumeMusic(mus_volume);
	return 0;
}

int mus_load(){
	DBG(3,"Loading music ...");
	music[0] = NULL;
	for(int i=1;i<mus_count;i++){
		DBG(5,"Loading %s from %s...",mus_names[i],mus_paths[i]);
		music[i] = Mix_LoadMUS(mus_paths[i]);	//Freed in audioCleanup or in playMusic
		if(music[i]==NULL){ERR(1,"Failed to load music from %s.",mus_paths[i]);}
	}
	DBG(3,"Music loaded.");
	return 0;
}

int sef_load(){
	DBG(3,"Loading sound effects...");
	soundEffects[0] = NULL;
	for(int i=1;i<sef_count;i++){
		DBG(5,"Loading %s from %s...",sef_names[i],sef_paths[i]);
		soundEffects[i] = Mix_LoadWAV(sef_paths[i]);
		if(soundEffects[i]==NULL){ERR(1,"Failed to load sound from %s.",sef_paths[i]);}
	}
	DBG(3,"Sound effects loaded.");
	return 0;
}

//Plays the given sound effect, modifying it according to the given volume multiplier (default 100) and applying the given effect (NYI).
int playSE(int i,int vol,int effect){
	if(i<0||i>=sef_count){ERR(1,"Sound effect not found.");}
	DBG(5,"Playing sound effect %s.",sef_names[i]);
	Mix_Volume(Mix_PlayChannel(-1,soundEffects[i],0),vol*sef_volume);
	return 0;
}

//Silences current music, abruptly if fade is 0 and fading out otherwise.
int silenceMusic(int fade){
	if(Mix_PlayingMusic()){
		DBG(5,"Fading out music.");
		if(fade){
			Mix_FadeOutMusic(100*fade);
		}else{
			Mix_HaltMusic();
		}
	}
	return 0;
}

//Plays music for a given number of loops, fading it over a period of time if fade isn't 0.
//If music is already playing, and fade is 0, it abruptly switches music. Otherwise it fades out the original music for half the period and fades in the new music for the rest, blocking until complete.
int playMusic(int i,int fade,int loops){
	if(i==0){silenceMusic(fade);}
	if(fade){
		DBG(5,"Fading in music %s.",mus_names[i]);
		if(Mix_PlayingMusic()){
			Mix_FadeOutMusic(50*fade);
			Mix_FadeInMusic(music[i],-1,50*fade);
		}else{
			Mix_FadeInMusic(music[i],-1,100*fade);
		}
	}else{
		DBG(5,"Playing music %s %d",mus_names[i],i);
		Mix_PlayMusic(music[i],loops-1);
	}
	return 0;
}

int audioCleanup(){
	for(int i=1;i<mus_count;i++){
		Mix_FreeMusic(music[i]);
	}
	for(int i=0;i<sef_count;i++){
		Mix_FreeChunk(soundEffects[i]);
	}
	Mix_Quit();
}