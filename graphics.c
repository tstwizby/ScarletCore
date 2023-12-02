//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Defines structures used to render graphical objects.
//Depends on SDL.h, font.c, and buffer.c, included in SDLWrapper.c
//NOTE a lot of things here are currently unused, and will be fully implemented in a future update which adds more functionality to graphical objects (e.g. adding non-background/scene images, creating text boxes etc.)
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef enum {obj_visible, obj_invisible} OBJ_STATUS;

//Structs defining graphical objects should end in 'G', and share their first elements with objG so they can be referenced generically
typedef struct _objG{
	SDL_Texture* txt;
	SDL_Rect rect;
	double angle;
	OBJ_STATUS status;
} objG;

//Text box obect definition and functions
#define TXTBOX_MARGIN_DEFAULT 5

typedef struct _TextBoxG{
	SDL_Texture* txt;
	SDL_Rect rect;
	double angle;
	OBJ_STATUS status;
	int lines;
	int cx;
	int cy;
	int margin;
	int font;
	int speed;
	int frame;
	Buffer* buffer;
} TextBoxG;

//Clears text buffer
int clearTextBox(TextBoxG* t){
	t->cx = t->margin;
	t->cy = t->margin;
	t->frame = 0;
	t->lines = 0;
	clearBuffer(t->buffer);
}

int buildTextBox(TextBoxG* t, int x, int y, int w, int h, int f, Buffer* source, SDL_Texture* txt){
	t->angle  = 0;
	t->status = obj_invisible;
	t->margin = TXTBOX_MARGIN_DEFAULT;
	t->font   = f;
	t->speed  = text_speed;
	t->txt    = txt;
	t->buffer = source;
	t->rect   = (SDL_Rect){x,y,w,h};
}

/*
Noteable sizes:

Backgrounds- 1920x1080
  3/4 scale- 1440x810
 half scale-  960x540
  1/4 scale-  480x270

       Text- 32px per line
 half scale- 16 px per line

   Text box: four lines, with a margin and a border
     height- 2px border + 6px margin + 4x32px line + 6px margin + 2px border = 144 px
      width- bg width -2px border on each side

	 arrows- 

total height = top margin + border + bg + border + text box + bottom margin
540+144+20+20

*/



const int bg_raw_width  = 1920;
const int bg_raw_height = 1080;

const int no_arrow_source_x = 0;
const int no_arrow_source_y = 0;
const int arrow_source_x = 0;
const int arrow_source_y[] = {
	0,42,84
};

const int arrow_width  = 82;
const int arrow_height = 42;

const int arrows_width  = 130;
const int arrows_height = 130;

const int up_arrow_x    =  24;
const int up_arrow_y    =  0;
const int down_arrow_x  =  24;
const int down_arrow_y  =  88;
const int left_arrow_x  = -20;
const int left_arrow_y  =  44;
const int right_arrow_x =  68;
const int right_arrow_y =  44;

int menu_height = 0;


