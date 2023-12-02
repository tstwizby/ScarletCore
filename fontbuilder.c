#include <stdio.h>
#include <stdlib.h>

char height = 32;
char width = 32;
char columns = 16;
char rows = 6;
char offset = 6;
char space = 10;

char widths[] = {
	32,19,13,32,32,32,32, 7,11,11, 8,32,18,11,18,12,
	20,19,19,19,20,19,20,19,20,20, 9,32,32,32,32,26,
	32,23,21,22,24,20,18,24,22, 8,10,23,19,26,22,25,
	20,24,22,20,22,22,23,32,22,22,22,32,32,32,32,32,
	32,18,20,17,19,19,13,19,19, 7, 8,19, 7,30,19,19,
	20,19,15,16,13,19,19,26,19,19,17,32,32,32,32,32
};


int main(){
	FILE* f = fopen("fonts\\testfont.dat","wb");
	fputc(height,f);
	fputc(width,f);
	fputc(columns,f);
	fputc(rows,f);
	fputc(offset,f);
	fputc(space,f);
	fputc(0,f);
	fputc(0,f);
	fwrite(widths,sizeof(*widths),columns*rows,f);
	fclose(f);
}