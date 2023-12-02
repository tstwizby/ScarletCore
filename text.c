//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Contains information about the static text boxes/script buffers used by the game.
//A bit thrown-together, will probably separate out script vs text logic at some point.
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int menuSize;
int menuSelect;

#define MAX_REC_DEPTH 4
#define TXTBOX_LEN_MAX 2048
#define MAX_MENU_OPTIONS 8

//Buffers for currently loaded script
uint8_t scrBuffer_buffer[SCRIPT_MAX_LENGTH];
Buffer scrBuffer;

//Buffer for options on currently-loaded menu
uint8_t menBuffer[MAX_MENU_OPTIONS][SCRIPT_MAX_LENGTH];
int menLen[MAX_MENU_OPTIONS];

//Information used to jump between scripts and return (via 'run' function, for example).
int currentScript = 0;
int scrDepth = 0;
int nextScript[MAX_REC_DEPTH];
int nextPos[MAX_REC_DEPTH];

//Used to determine if a 'time-based' script should run, such as one set to run when the time passes into evening for example.
int timequeue = 0;

//Debug tool for tracking which scripts are currently running.
int diagnoseScriptBuffers(){
	DBG(5,"Diagnosing script buffers...");
	DBG(7,"Currently at depth %d.",scrDepth);
	for(int i=scrDepth;i>0;i--){
		DBG(7,"Script at depth %d: %s (pos %d)",i,scr_names[nextScript[i]],nextPos[i]);
	}
	DBG(7,"Root script: %s (pos %d)",scr_names[nextScript[0]],nextPos[0]);
}

TextBox txtbox;
TextBox txtmenu;
Buffer txtboxBuffer;
Buffer txtmenuBuffer;
uint8_t txtbox_buffer[TXTBOX_LEN_MAX];
uint8_t txtmenu_buffer[TXTBOX_LEN_MAX];

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Setup-related functions
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Initialze the various static text boxes/buffers.
int setupBuffers(){
	buildBuffer(&scrBuffer,SCRIPT_MAX_LENGTH,scrBuffer_buffer);
	buildBuffer(&txtboxBuffer,TXTBOX_LEN_MAX,txtbox_buffer);
	buildBuffer(&txtmenuBuffer,TXTBOX_LEN_MAX,txtmenu_buffer);
	buildTextbox(&txtbox,res->txt_width,res->txt_height,&txtboxBuffer,txtBoxTxt);
	buildTextbox(&txtmenu,res->menu_width,0,&txtmenuBuffer,menuTxt);
	txtmenu.speed = 0;
}

//Clears text buffer and rendered text.
int clearTextbox(TextBox* t){
	t->cx    = t->margin;
	t->cy    = t->margin;
	t->frame = 0;
	t->lines = 0;
	clearBuffer(t->buffer);
	clearTxt(t->txt,clr_black);
}

//Draws a character to the given textbox using its set font.
int writeChar(TextBox* t){
	uint8_t ch = tickBuffer(t->buffer);
	if(ch == ctrl_space){
		t->cx += fonts[t->font].space;
	}else if(ch == ctrl_line){
		t->cx =  t->margin;
		t->cy += fonts[t->font].height;
	}else{
		ch-=ctrl_max;
		if(ch>=fonts[t->font].columns*fonts[t->font].rows || ch<0){ch=0;}
		int width  = fonts[t->font].charwidth[ch];
		int height = fonts[t->font].height;
		int fx = (ch%fonts[t->font].columns)*fonts[t->font].width;
		int fy = (ch/fonts[t->font].columns)*fonts[t->font].height;// + fonts[t->font].voffset;
		SDL_SetRenderTarget(renderer,t->txt);
		drawClippedTexture(fontTxt[t->font],t->cx,t->cy,fx,fy,width,height);
		SDL_SetRenderTarget(renderer,NULL);
		t->cx += width;
	}
	return 0;
}
//Instantly writes all text in the buffer.
int writeAllText(TextBox* t){
	while(countBuffer(t->buffer)){writeChar(t);}
}

//Advances the textbox one frame.
int tickText(TextBox* t){
	if(countBuffer(t->buffer)&&(t->frame==0)){
		t->frame=t->speed;
		writeChar(t);
	}else{t->frame--;}
}

//Loads text from script if any, returns number of loaded characters, including any added line breaks, or -1 if the buffer size of the textbox was exceeded.
int loadText(TextBox* t){
	uint8_t *data = readBuffer(&scrBuffer);
	if(data[0]<=ctrl_max&&!((data[0]==ctrl_line)||(data[0]==ctrl_space))){return 0;}
	int offset = t->buffer->len;
	int cpos   = t->cx;
	int i = 0;
	while((data[i]>ctrl_max)||(data[i]==ctrl_line)||(data[i]==ctrl_space)){
		//If the next word won't fit on the current line (and isn't the first word on the line), insert a new line.
		if(data[i]==ctrl_space){
			t->buffer->data[offset++] = ctrl_space;
			cpos += fonts[t->font].space;
			i++;
		}else if(data[i] == ctrl_line){
			t->buffer->data[offset]=ctrl_line;
			cpos = t->margin;
			i++;
		}else{
			int wordlen = wordLength(t->font,data+i);
			if((cpos>(t->margin))&&(cpos+wordlen>((t->width)-(t->margin)))){
				t->buffer->data[offset++] = ctrl_line;
				cpos = wordlen+t->margin;
			}else{cpos += wordlen;}
			while(data[i]>ctrl_max){
				t->buffer->data[offset++] = data[i++];
			}
		}
		if(offset>TXTBOX_LEN_MAX){ERR(-1,"Textbox buffer length exceeded.");}
	}
	t->buffer->len = offset;
	t->frame = t->speed;
	scrBuffer.pos += i-1;
	if(t->speed==0){writeAllText(t);}
	return offset;
}

//Adds text to the given textbox from the given source, adding line breaks where necessary and returning the final length of the text box or -1 if the buffer size was exceeded.
//Probably redundant with loadText in some way, but haven't thought of a workaround yet for their slightly different behavior.
int addText(TextBox* t, uint8_t* data){
	if(data[0]<=ctrl_max&&!((data[0]==ctrl_line)||(data[0]==ctrl_space))){return 0;}
	int offset = t->buffer->len;
	int cpos   = t->cx;
	int i = 0;
	while((data[i]>ctrl_max)||(data[i]==ctrl_line)||(data[i]==ctrl_space)){
		//If the next word won't fit on the current line (and isn't the first word on the line), insert a new line.
		if(data[i]==ctrl_space){
			t->buffer->data[offset++] = ctrl_space;
			cpos += fonts[t->font].space;
			i++;
		}else if(data[i] == ctrl_line){
			t->buffer->data[offset]=ctrl_line;
			cpos = t->margin;
			i++;
		}else{
			int wordlen = wordLength(t->font,data+i);
			if((cpos>(t->margin))&&(cpos+wordlen>((t->width)-(t->margin)))){
				t->buffer->data[offset++] = ctrl_line;
				cpos = wordlen+t->margin;
			}else{cpos += wordlen;}
			while(data[i]>ctrl_max){
				t->buffer->data[offset++] = data[i++];
			}
		}
		if(offset>TXTBOX_LEN_MAX-1){ERR(-1,"Textbox buffer length exceeded.");}
	}
	t->buffer->data[offset] = ctrl_line;
	t->lines++;
	offset++;
	t->buffer->len = offset;
	t->frame = t->speed;
	if(t->speed==0){writeAllText(t);}
	return offset;
}

//Renders the menu textbox to the screen along with the selection indicator.
int renderMenu(){
	drawClippedTexture(menuTxt,res->menu_x,res->menu_y,0,0,txtmenu.width,txtmenu.height);
	int s_x = res->menu_x+txtmenu.width-30;
	int s_y = res->menu_y+5+fonts[txtmenu.font].height*menuSelect;
	SDL_Rect rect = {s_x, s_y, 20, 20};
	SDL_SetRenderDrawColor(renderer,GET_COLOR(clr_white));
	SDL_RenderFillRect(renderer,&rect);
}


//Loads a script into the buffer.
int loadScript(int index){
	clearBuffer(&scrBuffer);
	if(index<0||index>=scr_count){ERR(1,"Script index out of bounds (%d)",index);}
	memcpy(scrBuffer_buffer,scr_buffers[index],scr_len[index]);
	scrBuffer.len = scr_len[index];
	scrBuffer_buffer[scr_len[index]] = ctrl_end;
	currentScript = index;
	status = RUNNING;
	DBG(5,"Script '%s' loaded",scr_names[index]);
	conv_script(scrBuffer.data,0);
}

//Loads a fast-travel script into the buffer.
int loadFastTravel(int dest, int dist){
	clearBuffer(&scrBuffer);
	scrBuffer.len = travelScript(dest,dist,scrBuffer_buffer);
	status = RUNNING;
	DBG(5,"Fast travel to %s",loc_names[dest]);
}

//Loads the hidden copyright script into the buffer.
int loadCopyright(){
	clearBuffer(&scrBuffer);
	scrBuffer.len = copyrightScript(scrBuffer_buffer);
	status = RUNNING;
}

//Loads a menu for display
int loadMenu(uint8_t* data){
	status = MENU;
	clearTextbox(&txtmenu);
	if(data[0]>MAX_MENU_OPTIONS){ERR(1,"Maximum number of menu options exceeded.");}
	int i = 1;
	int count = 0;
	while(data[i]!=ctrl_end){
		addText(&txtmenu,data+i);
		while(data[i]!=ctrl_delim){i++;}
		i++;
		int j = 0;
		while(data[i+j]!=ctrl_menu&&data[i+j]!=ctrl_end){
			if(data[i+j]==ctrl_func){j += func_arg_count[data[i+j+1]]+1;}
			j++;
		}
		memcpy(menBuffer[count],data+i,j);
		menBuffer[count][j] = ctrl_end;
		menLen[count] = j;
		count++;
		i += j+1;
	}
	menuSize = count;
	menuSelect = 0;
	txtmenu.height = txtmenu.margin*2+fonts[txtmenu.font].height*txtmenu.lines;
	writeAllText(&txtmenu);
	renderMenu();
}

//Runs the currently selected menu option
int selectMenu(){
	clearBuffer(&scrBuffer);
	DBG(5,"Menu selected: %d",menuSelect);
	memcpy(scrBuffer_buffer,menBuffer[menuSelect],menLen[menuSelect]);
	scrBuffer.len = menLen[menuSelect]+1;
	scrBuffer_buffer[menLen[menuSelect]] = ctrl_end;
	status = RUNNING;
	clearTextbox(&txtbox);
	clearTextbox(&txtmenu);
}

//Halts the currently running script and loads the next one and returns 0 if applicable, returns 1 otherwise
int stopScript(){
	DBG(5,"Stopping script");
	if(scrDepth>0){
		DBG(5,"Returning to script %d, depth %d, pos %d",nextScript[scrDepth-1],scrDepth-1,nextPos[scrDepth]);
		loadScript(nextScript[--scrDepth]);
		scrBuffer.pos=nextPos[scrDepth];
		return 0;
	}else{return 1;}
}

//Executes a function from the current script. Intended to be passed scrBuffer+offset for some offset. Return value depends on the function, returns -1 if no function is recognized.
int exec(uint8_t *params){
DBG(5,"executing %s",func_names[params[0]]);
	scrBuffer.pos+=func_arg_count[params[0]];
	switch(params[0]){
		case f_null: return 0;
		case f_quit: status = CLOSED; return 1;
		case f_stop: return stopScript();
		case f_goto: return loadScript(params[1]);
		case f_run:
			if(scrDepth==MAX_REC_DEPTH){ERR(2,"Maximum recurrence depth reached. Use more gotos and fewer runs.");}
			nextScript[scrDepth] = currentScript;
			nextPos[scrDepth] = scrBuffer.pos;
			scrDepth++;
			loadScript(params[1]);
			return 0;
		case f_runback:
			if(scrDepth==MAX_REC_DEPTH){ERR(2,"Maximum recurrence depth reached. Use more gotos and fewer runs.");}
			nextScript[scrDepth] = currentScript;
			nextPos[scrDepth] = -1;
			scrDepth++;
			loadScript(params[1]);
			return 0;
		case f_fadeout:
			status = PAUSED;
			return fadeOut(params[1],params[2]);
		case f_fadein:
			status = PAUSED;
			return fadeIn(params[1]);
		case f_transition:
			location = params[1];
			DBG(5,"Loading location %d %s",params[1],loc_names[params[1]]);
			//diagnoseLoc(cLocation);
			return loadBackground();
		case f_playBGM:
			return playMusic(params[1],params[2],params[3]);
		case f_playSE:
			return playSE(params[1],params[2],params[3]);
		case f_silence:
			return silenceMusic(params[1]);
		case f_addtime:
			timequeue = addTime(params[1],params[2]);
			if(timequeue&&scrDepth < MAX_REC_DEPTH){
				nextScript[scrDepth] = timequeue;
				nextPos[scrDepth] = -1;
				scrDepth++;
				DBG(5,"Adding time-based script '%s'",scr_names[timequeue]);
				timequeue = 0;
			}
			return 0;
		case f_setcfg:
			cfgVars[params[1]]=params[2];
			if(params[1]==cfg_mus_volume){resetAudio();}
			if(params[1]==cfg_text_speed){txtbox.speed = text_speed;}
			if(params[1]==cfg_resolution){
				setResolution(cfgVars[cfg_resolution]);
				setTextbox(&txtbox,res->txt_width,res->txt_height);
				setTextbox(&txtbox,res->menu_width,0);
				renderAll();
			}
			return 0;
		case f_setvar:
			gameVars[params[1]]=(*vop_funcs[params[2]])(gameVars[params[1]],params[3]);
			return 0;
		case f_checkvar:
			if(!(*chk_funcs[params[2]])(gameVars[params[1]],params[3])){return 0;}
			if(scrDepth==MAX_REC_DEPTH){ERR(2,"Maximum recurrence depth reached. Use more gotos and fewer runs.");}
			nextScript[scrDepth] = currentScript;
			nextPos[scrDepth] = scrBuffer.pos;
			scrDepth++;
			loadScript(params[4]);
			return 0;
		case f_switchvar:
			if(!(*chk_funcs[params[2]])(gameVars[params[1]],params[3])){return 0;}
			return loadScript(params[4]);
		case f_setlocscript:
			DBG(7,"Setting location '%s' data element '%s' to script %d",loc_names[params[1]],lde_names[params[2]],params[3]);
			switch(params[2]){
				case lde_enter:
					locations[params[1]].script = params[3];
					return 0;
				case lde_act:
					locations[params[1]].action = params[3];
					return 0;
				case lde_ex:
					locations[params[1]].examine = params[3];
					return 0;
				case lde_up:
					locations[params[1]].up = params[3];
					locations[params[1]].u_dist = -1;
					if(params[4]){locations[params[1]].u_arr=params[3]?params[4]:0;}
					return 0;
				case lde_down:
					locations[params[1]].down = params[3];
					locations[params[1]].d_dist = -1;
					if(params[4]){locations[params[1]].d_arr=params[3]?params[4]:0;}
					return 0;
				case lde_left:
					locations[params[1]].left = params[3];
					locations[params[1]].l_dist = -1;
					if(params[4]){locations[params[1]].l_arr=params[3]?params[4]:0;}
					return 0;
				case lde_right:
					locations[params[1]].right = params[3];
					locations[params[1]].r_dist = -1;
					if(params[4]){locations[params[1]].r_arr=params[3]?params[4]:0;}
					return 0;
				default:
					ERR(-1,"Unrecognized location data element encountered (%d)",params[2]);
			}
		case f_setlocdest:
			if(params[1]>=loc_count){ERR(-1,"Target location ID out of range");}
			if(params[2]>=dir_count){ERR(-1,"Unrecognized direction");}
			if(params[3]>=loc_count){ERR(-1,"Destination location ID out of range");}
			if(params[4]<0){ERR(-1,"Negative distances not currently permitted");}
			DBG(5,"Setting location '%s' destination '%s' to '%s' at distance '%d'",loc_names[params[1]],dir_names[params[2]],loc_names[params[3]],params[4]);
			switch(params[2]){
				case dir_up:
					locations[params[1]].up=params[3];
					locations[params[1]].u_dist=params[4];
					locations[params[1]].u_arr=params[3]?1:0;
					return 0;
				case dir_down:
					locations[params[1]].down=params[3];
					locations[params[1]].d_dist=params[4];
					locations[params[1]].d_arr=params[3]?1:0;
					return 0;
				case dir_left:
					locations[params[1]].left=params[3];
					locations[params[1]].l_dist=params[4];
					locations[params[1]].l_arr=params[3]?1:0;
					return 0;
				case dir_right:
					locations[params[1]].right=params[3];
					locations[params[1]].r_dist=params[4];
					locations[params[1]].r_arr=params[3]?1:0;
					return 0;
			}
			return -1;
		case f_wait:
			DBG(7,"Waiting for %d frames...",params[1]);
			delay = params[1];
			status = WAITING;
			return 0;
		case f_setscene:
			return loadScene(params[1]);
		default:
			ERR(-1,"Unrecognized function code encountered (%d)",params[0]);
	}
	return 0;
}

//Evaluates the next unit of the current script
int scrNext(){
	switch(tickBuffer(&scrBuffer)){
		case ctrl_func:
			tickBuffer(&scrBuffer);
			return exec(readBuffer(&scrBuffer));
		case ctrl_menu:
			tickBuffer(&scrBuffer);
			return loadMenu(readBuffer(&scrBuffer));
		case ctrl_clear:
			clearTextbox(&txtbox);
			return 0;
		case ctrl_wait:
			status = WAITING;
			return 0;
		case ctrl_end: return stopScript();
		case ctrl_var:
			//NYI
			return 0;
		default:
			loadText(&txtbox);
			return 0;
	}
}








