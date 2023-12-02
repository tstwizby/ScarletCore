//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Defines the 'Buffer' struct and associated functions.
//Depends on inttypes.h
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct _Buffer{
	int len;			//Size of buffer
	int pos;			//Current read position in buffer
	uint8_t* data;		//Data in buffer
} Buffer;

//Returns a pointer to the buffer's data at its current position.
uint8_t* readBuffer(Buffer* buffer){
	return (buffer->data)+(buffer->pos);
}

//Increments the buffer's position and returns the data at the new position.
uint8_t tickBuffer(Buffer* buffer){
	return buffer->data[++(buffer->pos)];
}

//Returns the number of bytes of data left to read in the buffer.
int countBuffer(Buffer* buffer){
	return (buffer->len)-(buffer->pos)-1;
}

//Resets the length/position of the buffer.
int clearBuffer(Buffer* buffer){
	buffer->len=0;
	buffer->pos=-1;
}

//Initializes the length/position and data storage for the buffer.
int buildBuffer(Buffer* buffer, int len, uint8_t* source){
	buffer->len = 0;
	while(buffer->len<len&&source[buffer->len]){buffer->len++;}
	buffer->pos;
	buffer->data=source;
}