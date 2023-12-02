//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Script compiler
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include "stringparse.c"
#include "error.c"
#include "paths.c"

#define MAX_MENU_OPTIONS 8
#define COMPILER 1

//contains game variable and config datatypes and names
#include "game.c"
//contains script function and macro definitions and names and defines character encoding, among other things
#include "script.c"

#define MAX_LABEL 64
#define BUFFER_LEN 2048
#define MAX_COUNT 256
#define MAX_RAW_SCRIPT_LENGTH 16384
#define LOC_BIN_LEN 18

#define MAX_FN_NAME_LEN 16

uint8_t* bin_scripts[MAX_SCR_COUNT];
uint16_t scrlengths[MAX_SCR_COUNT];
uint8_t bin_locs[MAX_LOC_COUNT][LOC_BIN_LEN];

typedef int (*registerFn)(const char*);

int readData(char* buffer, registerFn reg){
	int i = 0;
	int ind = 0;
	while(buffer[ind+i]){
		while(buffer[ind+i]&&isspace(buffer[ind+i])){ind++;}
		if(!buffer[ind+i]){return 0;}
		while(!isspace(buffer[ind+i])){i++;}
		buffer[ind+i] = 0;
		if(reg(buffer+ind)){return 1;}
		ind+=i+1;
		i = 0;
	}
	return 0;
}

int getData(FILE* raw){
	char buffer[BUFFER_LEN];
	//skip label line
	if(!fgets(buffer,BUFFER_LEN,raw)){ERR(1,"Improperly formatted raw data file.");}
	if(!fgets(buffer,BUFFER_LEN,raw)){ERR(1,"Improperly formatted raw data file.");}
	//read vars
	DBG(1,"Reading variable data...");
	if(readData(buffer, var_register)){return 1;}
	//skip label line
	if(!fgets(buffer,BUFFER_LEN,raw)){ERR(1,"Improperly formatted raw data file.");}
	if(!fgets(buffer,BUFFER_LEN,raw)){ERR(1,"Improperly formatted raw data file.");}
	//read scenes
	DBG(1,"Reading scene data...");
	if(!fgets(buffer,BUFFER_LEN,raw)){ERR(1,"Improperly formatted raw data file.");}
	if(readData(buffer, scn_register)){return 1;}
	//skip label line
	if(!fgets(buffer,BUFFER_LEN,raw)){ERR(1,"Improperly formatted raw data file.");}
	if(!fgets(buffer,BUFFER_LEN,raw)){ERR(1,"Improperly formatted raw data file.");}
	//read music
	DBG(1,"Reading music data...");
	if(!fgets(buffer,BUFFER_LEN,raw)){ERR(1,"Improperly formatted raw data file.");}
	if(readData(buffer, mus_register)){return 1;}
	//skip label line
	if(!fgets(buffer,BUFFER_LEN,raw)){ERR(1,"Improperly formatted raw data file.");}
	if(!fgets(buffer,BUFFER_LEN,raw)){ERR(1,"Improperly formatted raw data file.");}
	//read music
	DBG(1,"Reading sound effect data...");
	if(!fgets(buffer,BUFFER_LEN,raw)){ERR(1,"Improperly formatted raw data file.");}
	if(readData(buffer, sef_register)){return 1;}
	return 0;
}

int getScripts(FILE* raw){
	int linecount = 0;
	char buffer[BUFFER_LEN] = {' '};
	while(isspace(buffer[0])&&fgets(buffer,BUFFER_LEN,raw)){linecount++;}
	int done = 0;
	while(!done){
		int i = 0;
		while(buffer[i]!=':'&&buffer[i]){
			i++;
		}
		if(buffer[i]!=':'){ERR(1,"Improperly formatted label: %s",buffer);}
		buffer[i]=0;
		if(scr_register(buffer)){ERR(2,"Could not register script on line %d",linecount);}
		if(!fgets(buffer,BUFFER_LEN,raw)||isspace(buffer[0])){ERR(1,"Improperly formatted script.");}
		while(!isspace(buffer[0])&&fgets(buffer,BUFFER_LEN,raw)){linecount++;}
		while(isspace(buffer[0])&&!done){if(!fgets(buffer,BUFFER_LEN,raw)){done = 1;} linecount++;}
	}
	return 0;
}

int getLocs(FILE* raw){
	int linecount = 0;
	char buffer[BUFFER_LEN];
	while(fgets(buffer,BUFFER_LEN,raw)){
		while(isspace(buffer[0])&&fgets(buffer,BUFFER_LEN,raw)){linecount++;}
		int i = 0;
		while(!isspace(buffer[i])){i++;}
		buffer[i]=0;
		if(loc_register(buffer)){ERR(2,"Could not register location on line %d",linecount);}
		if(!fgets(buffer,BUFFER_LEN,raw)||isspace(buffer[0])){ERR(1,"Missing or improperly formatted enter script.");}
		if(!fgets(buffer,BUFFER_LEN,raw)||isspace(buffer[0])){ERR(1,"Missing or improperly formatted action script.");}
		if(!fgets(buffer,BUFFER_LEN,raw)||isspace(buffer[0])){ERR(1,"Missing or improperly formatted examine script.");}
		if(!fgets(buffer,BUFFER_LEN,raw)||!isspace(buffer[0])){ERR(1,"Extra lines found for location");}
	}
}

int parseScript(char* src, int sc){
	DBG(1,"Parsing script '%s'",scr_names[sc]);
	DBG(1,"Raw script: '%s'",src);
	int err = 0;
	//raw index
	int i = 0;
	//binary index
	int j = 0;
	//menu status
	int mmode = 0;
	uint8_t dest[SCRIPT_MAX_LENGTH] = {0};
	while(src[i]){
		switch(src[i]){
			case '@':
				//wait for input, then clear the textbox
				dest[j]   = ctrl_wait;
				dest[j+1] = ctrl_clear;
				j+=2;
				break;
			case '{':
				//function block
				DBG(1,"Starting function block...");
				int funcs = encCount(src+i,'{','}');
				if(funcs==0){ERR(4,"Empty or improperly formatted function block");}
				char** block_funcs = malloc((sizeof (*block_funcs))*funcs);
				if(block_funcs==NULL){ERR(3,"Problem allocating memory for function block");}
				if(enclosed(block_funcs,src+i,'{','}')!=funcs){
					DBG(1,"Error reading function block.");
					err = 3;
					goto cleanup;
				}
				//parse functions within block
				for(int k=0;k<funcs;k++){
					char fname[MAX_FN_NAME_LEN] = {0};
					//index within function
					int f = 0;
					while(f<MAX_FN_NAME_LEN&&block_funcs[k][f]!='('&&block_funcs[k][f]){
						fname[f]=block_funcs[k][f];
						f++;
					}
					int fid = searchNames(type_func,fname);
					if(fid<0){
						//not a function name; either an error, or a macro
						int mid = searchNames(type_smac,fname);
						if(mid<0){
							DBG(1,"Unrecognized function or macro name: %s",fname);
							err = 4;
							goto cleanup;
						}
						DBG(1,"\tMacro '%s':",fname);
						int margc = smac_arg_count[mid];
						uint8_t margs[margc];
						if(block_funcs[k][f]!='('){
							DBG(1,"Improperly formatted macro: %s",block_funcs[k]);
							err = 4;
							goto cleanup;
						}
						f++;
						for(int ac=0;ac<margc;ac++){
							//argument index
							int ai = 0;
							char* carg = calloc(MAX_LABEL,sizeof(*carg));
							while(block_funcs[k][f]!=','&&block_funcs[k][f]!=')'&&block_funcs[k][f]){
								carg[ai]=block_funcs[k][f];
								f++;
								ai++;
							}
							margs[ac] = searchNames(smac_arg_types[mid][ac],carg);
							if(margs[ai]<0){
								DBG(1,"Incorrect parameter type for macro (expected type '%s'): %s",type_names[smac_arg_types[mid][ac]],carg);
								err = 4;
							}else if(margs[ai]>255){
								DBG(1,"No currently implemented macro uses integers larger than one byte.");
								err = 4;
							}
							if(ac+1!=margc&&block_funcs[k][f]!=','){
								DBG(1,"Missing parameters for macro: %s",block_funcs[k]);
								err = 4;
							}else if(ac+1==margc&&block_funcs[k][f]!=')'){
								DBG(1,"Too many parameters or missing ')' for macro: %s",block_funcs[k]);
								err = 4;
							}
							DBG(1,"\t\tParameter %d '%d' (%s)",ac,margs[ac],carg);
							free(carg);
							f++;
							if(err>0){goto cleanup;}
						}
						DBG(1,"\tExpanding macro '%s'",smac_names[mid]);
						for(int fc=0;fc<smac_size[mid];fc++){
							int sfunc = smac_expand[mid][fc];
							dest[j]=ctrl_func;
							j++;
							dest[j]=(uint8_t)sfunc;
							DBG(1,"\t\tFunction '%s'",func_names[sfunc]);
							j++;
							for(int fac=0;fac<func_arg_count[sfunc];fac++){
								fc++;
								if(smac_expand[mid][fc]<0){
									dest[j]=(uint8_t)margs[-1-smac_expand[mid][fc]];
									DBG(1,"\t\t\tParam %d (%d)",*(dest+j),-1-smac_expand[mid][fc]);
								}else{
									dest[j]=(uint8_t)smac_expand[mid][fc];
									DBG(1,"\t\t\tParam %d",smac_expand[mid][fc]);
								}
								j++;
							}
						}
					}else{
						dest[j]=ctrl_func;
						j++;
						dest[j]=(uint8_t)fid;
						j++;
						int argc = func_arg_count[fid];
						DBG(1,"\tFunction '%s'",func_names[fid]);
						if(argc>0){
							if(block_funcs[k][f]!='('){
								DBG(1,"Improperly formatted function: %s",block_funcs[k]);
								err = 4;
								goto cleanup;
							}
							f++;
							for(int ac=0;ac<argc;ac++){
								int ai = 0;
								char* carg = calloc(MAX_LABEL,sizeof (*carg));
								while(block_funcs[k][f]!=','&&block_funcs[k][f]!=')'&&block_funcs[k][f]){
									carg[ai]=block_funcs[k][f];
									f++;
									ai++;
								}
								int argval = searchNames(func_arg_types[fid][ac],carg);
								if(argval<0){
									DBG(1,"Incorrect parameter type for function (expected type '%s'): '%s'",type_names[func_arg_types[fid][ac]],carg);
									err = 4;
								}else{
									if(argval>255){
										DBG(1,"No currently implemented function uses integers larger than one byte.");
										err = 4;
									}
									dest[j]=(uint8_t)argval;
									j++;
								}
								if(ac+1!=argc&&block_funcs[k][f]!=','){
									DBG(1,"Missing parameters for function (need %d): %s",argc,block_funcs[k]);
									err = 4;
								}
								if(ac+1==argc&&block_funcs[k][f]!=')'){
									DBG(1,"Too many parameters or missing ')' for function: %s",block_funcs[k]);
									err = 4;
								}
								DBG(1,"\t\tParameter %d '%d' (%s)",ac,argval,carg);
								free(carg);
								if(err>0){
									goto cleanup;
								}
								f++;
							}
						}else{
							f++;
							f++;
						}
					}
				}
				cleanup:
				for(int k=0;k<funcs;k++){free(block_funcs[k]);}
				free(block_funcs);
				if(err>0){return err;}
				i+=strcspn(src+i,"}");
				break;
			case '>':
				//menu
				dest[j++]=ctrl_menu;
				if(!mmode){
					int mcount = 1;
					int k=0;
					while(src[i+k++]){
						if(src[i+k]=='>'){mcount++;}
					}
					if(mcount>MAX_MENU_OPTIONS){ERR(4,"Too many menu options");}
					dest[j++]=(uint8_t)mcount;
					mmode = 1;
				}
				i++;
				//Fill menu label
				while(src[i]&&src[i]!='|'){dest[j++]=convert(src[i++]);}
				if(!src[i]){ERR(4,"Menu missing delimiter");}
				dest[j++]=ctrl_delim;
				break;
			case '[':
				//variable
				src[i+strcspn(src+i,"]")]=0;
				int v = searchNames(type_var,src+i+1);
				if(v<0){ERR(4,"Unrecognized variable name '%s'",src+i+1);}
				dest[j++]=ctrl_var;
				dest[j++]=(uint8_t)v;
				while(src[i]){i++;}
				break;
			default:
				//displayable character
				dest[j++]=convert(src[i]);
		}
		i++;
	}
	scrlengths[sc] = j;
	bin_scripts[sc] = malloc(sizeof(*bin_scripts)*j);
	memcpy(bin_scripts[sc],dest,j);
	return 0;
}

int parseScripts(FILE* raw){
	int linecount = 0;
	int sc = 1;
	while(sc<scr_count){
		char buffer[BUFFER_LEN] = {0};
		//read next script into buffer
		while(fgets(buffer,BUFFER_LEN,raw)&&isspace(buffer[0])){linecount++;}
		linecount++;
		if(!fgets(buffer,BUFFER_LEN,raw)||isspace(buffer[0])){ERR(1,"Problem parsing script (near line %d)",linecount);}
		linecount++;
		int slen = strcspn(buffer,"\r\n");
		char bufferB[BUFFER_LEN];
		while(fgets(bufferB,BUFFER_LEN,raw)&&!isspace(bufferB[0])){
			linecount++;
			int blen = strcspn(bufferB,"\r\n");
			if(slen+blen>=BUFFER_LEN){ERR(1,"Script near line %d exceeds raw script length limits",linecount);}
			memcpy(buffer+slen,bufferB,blen);
			slen+=blen;
		}
		if(slen>=MAX_RAW_SCRIPT_LENGTH){ERR(1,"Script near line %d exceeds raw script length limits",linecount);}
		if(parseScript(buffer,sc)){ERR(2,"(Near line %d)",linecount);}
		sc++;
		linecount++;
	}
	return 0;
}

#define getDir(D) 	code*=2; if(compare(locParams[j],"none"))	{D=0; d##D=0;DBG(1,"No "#D" neigbor");}\
								else if(locParams[j][0]==':')	{D=searchNames(type_scr,locParams[j]+1); if(D<0){ERR(3,"Unrecognized script '%s'",locParams[j]);} d##D=0;DBG(1,"Script %s in direction "#D,locParams[j]);}\
								else							{D=searchNames(type_loc,locParams[j]); if(D<0){ERR(3,"Unrecognized location '%s'",locParams[j]);} code++;DBG(1,"Location %s in direction "#D,locParams[j]);\
																j++; d##D = searchNames(type_num,locParams[j]); if(d##D<0){ERR(3,"Missing/incorrect distance information in direction "#D);}DBG(1,"Distance %d",d##D);} j++;

int parseLocs(FILE* raw){
	int lc = 0;
	char buffer[BUFFER_LEN];
	while(lc<loc_count&&fgets(buffer,BUFFER_LEN,raw)){
		DBG(1,"Parsing location '%s'...",loc_names[lc]);
		while(isspace(buffer[0])){fgets(buffer,BUFFER_LEN,raw);}
		char* locParams[12];
		int i = 0;
		int j = 0;
		while(buffer[i]){
			int k = 0;
			while(!isspace(buffer[i+k])){k++;}
			buffer[i+k]=0;
			locParams[j++]=buffer+i;
			while(isspace(buffer[i+k])){k++;}
			i+=k;
			i++;
		}
		j=1;
		uint8_t card = (uint8_t)searchNames(type_crd,locParams[j++]);
		uint8_t sky = (uint8_t)searchNames(type_sky,locParams[j]);
		if(sky==255){sky = (uint8_t)searchNames(type_num,locParams[j]);}
		j++;
		uint8_t shade = (uint8_t)searchNames(type_shd,locParams[j]);
		if(shade==255){shade = (uint8_t)searchNames(type_num,locParams[j]);}
		j++;
		uint8_t u;
		uint8_t d;
		uint8_t l;
		uint8_t r;
		uint8_t du;
		uint8_t dd;
		uint8_t dl;
		uint8_t dr;
		uint8_t code = 0;
		getDir(u);
		getDir(d);
		getDir(l);
		getDir(r);
		bin_locs[lc][0] =card;
		bin_locs[lc][1] =sky;
		bin_locs[lc][2] =shade;
		bin_locs[lc][3] =u;
		bin_locs[lc][4] =du;
		bin_locs[lc][5] =d;
		bin_locs[lc][6] =dd;
		bin_locs[lc][7] =l;
		bin_locs[lc][8] =dl;
		bin_locs[lc][9] =r;
		bin_locs[lc][10]=dr;
		bin_locs[lc][11]=code;
		DBG(1,"Getting scripts...");
		if(!fgets(buffer,BUFFER_LEN,raw)){ERR(2,"Could not read location scripts");}
		if(!buffer[0]||isspace(buffer[0])){ERR(3,"Missing location scripts");}
		buffer[strcspn(buffer,"\r\n")]=0;
		if(buffer[0]==':'){
			//raw script
			char scname[MAX_LOC_NAME_LEN+4] = {0};
			i = loc_name_lengths[lc];
			memcpy(scname,loc_names[lc],i);
			scname[i++]='_';
			scname[i++]='s';
			scname[i++]='c';
			scname[i]  ='r';
			if(scr_register(scname)){ERR(3,"Location script name collision: %s",scname);}
			if(parseScript(buffer+1,scr_count-1)){return 3;}
			bin_locs[lc][12] = (uint8_t)((scr_count-1)/256);
			bin_locs[lc][13] = (uint8_t)((scr_count-1)%256);
		}else{
			//existing script
			int lscr = searchNames(type_scr,buffer);
			if(lscr<0){ERR(3,"Unrecognized script: %s",buffer);}
			bin_locs[lc][12] = (uint8_t)(lscr/256);
			bin_locs[lc][13] = (uint8_t)(lscr%256);
		}
		if(!fgets(buffer,BUFFER_LEN,raw)){ERR(2,"Could not read action script");}
		if(!buffer[0]||isspace(buffer[0])){ERR(3,"Missing action script");}
		buffer[strcspn(buffer,"\r\n")]=0;
		if(buffer[0]==':'){
			//raw script
			char scname[MAX_LOC_NAME_LEN+4] = {0};
			i = loc_name_lengths[lc];
			memcpy(scname,loc_names[lc],i);
			scname[i++]='_';
			scname[i++]='a';
			scname[i++]='c';
			scname[i]  ='t';
			if(scr_register(scname)){ERR(3,"Location action script name collision: %s",scname);}
			if(parseScript(buffer+1,scr_count-1)){return 3;}
			bin_locs[lc][14] = (uint8_t)((scr_count-1)/256);
			bin_locs[lc][15] = (uint8_t)((scr_count-1)%256);
		}else{
			int lscr = searchNames(type_scr,buffer);
			if(lscr<0){ERR(3,"Unrecognized action script: %s",buffer);}
			bin_locs[lc][14] = (uint8_t)(lscr/256);
			bin_locs[lc][15] = (uint8_t)(lscr%256);
		}
		if(!fgets(buffer,BUFFER_LEN,raw)){ERR(2,"Could not read examine script");}
		if(!buffer[0]||isspace(buffer[0])){ERR(3,"Missing examine script");}
		buffer[strcspn(buffer,"\r\n")]=0;
		if(buffer[0]==':'){
			//raw script
			char scname[MAX_LOC_NAME_LEN+4] = {0};
			i = loc_name_lengths[lc];
			memcpy(scname,loc_names[lc],i);
			scname[i++]='_';
			scname[i++]='e';
			scname[i++]='x';
			scname[i]  ='a';
			if(scr_register(scname)){ERR(3,"Location examine script name collision: %s",scname);}
			if(parseScript(buffer+1,scr_count-1)){return 3;}
			bin_locs[lc-1][16] = (uint8_t)((scr_count-1)/256);
			bin_locs[lc][17] = (uint8_t)((scr_count-1)%256);
		}else{
			int lscr = searchNames(type_scr,buffer);
			if(lscr<0){ERR(3,"Unrecognized examine script: %s",buffer);}
			bin_locs[lc][16] = (uint8_t)(lscr/256);
			bin_locs[lc][17] = (uint8_t)(lscr%256);
		}
		lc++;
	}
	return 0;
}

int cleanup(){
	for(int i=0;i<scr_count;i++){free(bin_scripts[i]);}
}

int saveScripts(){
	FILE* dest = fopen(script_bin,"wb");
	DBG(1,"Writing compiled scripts...");
	if(dest==NULL){ERR(1,"Could not write to file");}
	DBG(1,"Writing %d total scripts",scr_count);
	for(int i=1;i<scr_count;i++){
		DBG(1,"Writing script %d '%s' (%d)...",i,scr_names[i],scrlengths[i]);
		writeInt(scrlengths[i], dest);
		fwrite(bin_scripts[i],1,scrlengths[i],dest);
	}
	fclose(dest);
	DBG(1,"Done");
	return 0;
}

int saveLocs(){
	FILE* dest = fopen(loc_bin,"wb");
	DBG(1,"Writing compiled locations...");
	if(dest==NULL){ERR(1,"Could not write to file");}
	for(int i=0;i<loc_count;i++){
		DBG(1,"Writing location %d '%s'",i,loc_names[i]);
		/*for(int j=0; j<18; j++){
			printf(" %d",bin_locs[i][j]);
		}printf("\n");*/
		fwrite(bin_locs[i],LOC_BIN_LEN,1,dest);
	}
	fclose(dest);
	DBG(1,"Done");
	return 0;
}

#define writeData(O, A, B) writeInt(A##_count,B); for(int i=O;i<A##_count;i++){uint8_t len=(uint8_t)strlen(A##_names[i]); fwrite(&len,1,1,B); fwrite(A##_names[i],1,len,B);} DBG(1,"Wrote %s name data (%d)",type_names[type_##A],A##_count)
int saveData(){
	FILE* dest = fopen(dat_bin,"wb");
	DBG(1,"Writing registered data...");
	if(dest==NULL){ERR(1,"Could not write to file");}
	//numbers here are the number of pre-registered standard things of each type
	writeData(1,scr,dest);
	writeData(0,loc,dest);
	writeData(1,mus,dest);
	writeData(1,sef,dest);
	writeData(1,scn,dest);
	writeData(5,var,dest);
	fclose(dest);
	DBG(1,"Done");
	return 0;
}

int main(){
	initGameValues();
	DBG(1,"Loading data from %s...",dat_raw);
	FILE* raw = fopen(dat_raw,"r");
	if(raw==NULL){ERR(1,"Could not open file.");}
	if(getData(raw)){fclose(raw);cleanup();return 2;}
	
	DBG(1,"Loading script names from %s...",script_raw);
	raw = fopen(script_raw,"r");
	if(raw==NULL){cleanup();ERR(1,"Could not open file.");}
	if(getScripts(raw)){fclose(raw);cleanup();return 2;}
	fclose(raw);
	
	DBG(1,"Loading loc names from %s...",loc_raw);
	raw = fopen(loc_raw,"r");
	if(raw==NULL){cleanup();ERR(1,"Could not open file.");}
	if(getLocs(raw)){fclose(raw);cleanup();return 2;}
	fclose(raw);
	
	raw = fopen(script_raw,"r");
	DBG(1,"Loading script data from %s...",script_raw);
	if(raw==NULL){cleanup();ERR(1,"Could not open file.");}
	if(parseScripts(raw)){fclose(raw);cleanup();return 2;}
	fclose(raw);
	raw = fopen(loc_raw,"r");
	DBG(1,"Loading loc data from %s...",loc_raw);
	if(raw==NULL){cleanup();ERR(1,"Could not open file.");}
	if(parseLocs(raw)){fclose(raw);cleanup();return 2;}
	fclose(raw);
	saveData();
	saveScripts();
	saveLocs();
	cleanup();
}