//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Contains information directly relevant to writing/reading script files.
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint8_t DEFAULT_TRANSITION_COLOR = clr_black;
uint8_t DEFAULT_TRANSITION_TIME = 100;

//Generates scripts for fast-travel.
int travelScript(int loc, int dist, uint8_t* dest){
	dest[0]  = ctrl_clear;
	dest[1]  = ctrl_func;
	dest[2]  = f_playSE;
	dest[3]  = sef_steps;
	dest[4]  = 100;
	dest[5]  = 0;
	dest[6]  = ctrl_func;
	dest[7]  = f_fadeout;
	dest[8]  = DEFAULT_TRANSITION_COLOR;
	dest[9]  = DEFAULT_TRANSITION_TIME;
	dest[10] = ctrl_func;
	dest[11] = f_addtime;
	dest[12] = 0;
	dest[13] = (uint8_t)dist;
	dest[14] = ctrl_func;
	dest[15] = f_transition;
	dest[16] = loc;
	dest[17] = ctrl_func;
	dest[18] = f_fadein;
	dest[19] = DEFAULT_TRANSITION_TIME;
	dest[20] = ctrl_clear;
	dest[21] = locations[loc].script?ctrl_func:ctrl_end;
	dest[22] = f_goto;
	dest[23] = locations[loc].script;
	dest[24] = ctrl_end;
	return 24;
}



//Converts from ascii to script format (not quite 1-1 at the moment)
uint8_t convert(uint8_t a){
	//Unrecognized character
	if((a<32&&a!=10)||a>128){return ctrl_space;}
	switch(a){
		case 32: return ctrl_space;
		case 10: return ctrl_line;
		default: return a;
	}
}

//Converts from script format to ascii (not quite 1-1 at the moment)
char unconvert(uint8_t a){
	switch(a){
		case ctrl_space: return 32;
		case  ctrl_line: return 10;
		        default: return a;
	}
}

//Hidden copyright script, breaks if misused or if COPYRIGHT_MESSAGE is longer than the script buffer size.
//COPYRIGHT_MESSAGE is formatted as a pure-text script, supporting newlines/spaces but not other control characters.
#define COPYRIGHT_MESSAGE "Copyright Peter Marchetti, 2023."
int copyrightScript(uint8_t* dest){
	int len = sizeof(COPYRIGHT_MESSAGE)+2;
	if(len>SCRIPT_MAX_LENGTH){ERR(1,"Could not display copyright message- intended message was '" COPYRIGHT_MESSAGE "'.");}
	dest[0]     = ctrl_clear;
	char* msg   = COPYRIGHT_MESSAGE;
	for(int i=0;i<len-2;i++){
		if     (msg[i]==' ' ){dest[i+1] = ctrl_space;}
		else if(msg[i]=='\n'){dest[i+1] = ctrl_line;}
		else{                 dest[i+1] = msg[i];}
	}
	dest[len-1] = ctrl_end;
	return len;
}

int conv_print(uint8_t* data, int max){
	int i = 0;
	while((i<max||max==0)&&data[i]/*&&(data[i]>ctrl_max||data[i]==ctrl_space||data[i]==ctrl_line)*/){
		printf("%c",unconvert(data[i]));
		i++;
	}
	printf("'\n");
}

int conv_script(uint8_t* data, int max){
	int i = 0;
	printf("scr:'");
	while(i<max||(max==0&&data[i])){
		if(data[i]<ctrl_max&&data[i]!=ctrl_space){
			printf("[%s]",ctrl_names[data[i]]);
			if(data[i]==ctrl_func){
				int f = data[++i];
				printf("[%s(",func_names[f]);
				for(int j=0;j<func_arg_count[f];j++){
					i++;
					int t = func_arg_types[f][j];
					printf("%d/",data[i]);
					if(t!=type_num){printf("%s ", all_names[t][data[i]]);}
					else{printf("%d ",data[i]);}
				}
				printf(")]");
			}else if(data[i]==ctrl_menu){
				printf("(%d)",data[++i]);
			}else if(data[i]==ctrl_var){
				printf("(%s)",var_names[data[++i]]);
			}
		}else{printf("%c",unconvert(data[i]));}
		i++;
	}
	printf("'\n");
}
