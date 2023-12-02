//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Defines the 'TextBox' struct and associated functions.
//Relies on 'buffer.c' and 'SDLWrapper.c'
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define TXTBOX_MARGIN_DEFAULT 5

typedef struct _TextBox{
	int width;				//width of text box
	int height;				//height of text box
	int lines;				//number of lines written to the textbox
	int cx;					//x position of the next character to be written to the text box
	int cy;					//y position of the next character to be written to the text box
	int margin;				//spacing between edge of text box and text
	int font;				//font for the textbox
	int speed;				//text speed of the text box (frames-per-character rendered, 0 for instant text)
	int frame;				//frame of textbox for displaying the next character
	Buffer* buffer;			//buffer for text written to the textbox
	SDL_Texture* txt;		//texture to which the textbox is rendered
} TextBox;

//Clears text buffer and resets the current textbox dimensions
int setTextbox(TextBox* t, int w, int h){
	t->width  = w;
	t->height = h;
	t->cx     = t->margin;
	t->cy     = t->margin;
	t->frame  = 0;
	t->lines  = 0;
	clearBuffer(t->buffer);
}

//Initializes textbox values to the default
int buildTextbox(TextBox* t, int w, int h, Buffer* source, SDL_Texture* txt){
	t->margin = TXTBOX_MARGIN_DEFAULT;
	t->font   = 0;
	t->speed  = text_speed;
	t->txt    = txt;
	t->buffer = source;
	setTextbox(t,w,h);
}



