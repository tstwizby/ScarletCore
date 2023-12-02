//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Handles errors and logging.
//Depends on stdio.h
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define debug_lv 10
#ifndef DEBUG_VERSION
#define DEBUG_VERSION 2
#endif

//Console logging
#if DEBUG_VERSION > 1
#define ERR(code, message,...) printf(message "\n", ##__VA_ARGS__); return code
#define DBG(lvl, message,...) if(debug_lv>lvl){printf(message "\n", ##__VA_ARGS__);}

//Log file
#elif DEBUG_VERSION == 1
#define ERR(code, message,...) fprintf(stderr,message "\n",##__VA_ARGS__); logMessage(message "\n",...); return code
#define DBG(lvl, message,...) if(debug_lv>lvl){logMessage(message "\n",...);}
const char* log_path = "log.txt";
int clearLog(){
	FILE* log = fopen(log_path,"w");
	if(!log){fprintf(stderr,"Could not write to log file '%s'!\n",log_path);return 1;}
	fprintf(log,"Starting log...\n");
	fclose(log);
}
int log(const char* message,...){
	FILE* log = fopen(log_path,"a");
	if(!log){fprintf(stderr,"Could not write to log file '%s'\n",log_path);return 1;}
	va_list argptr;
	va_start(argptr,message);
	fprintf(log,message,argptr);
	fclose(log);
	va_end(argptr);
	return 0;
}

//No logging
#else
#define ERR(code, message,...) fprintf(stderr,message "\n",##__VA_ARGS__); return code
#define DBG(message,...)
#endif

