//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Defines the format for file paths and provides a number of helper macros for file IO.
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef WINDOWS
#define WINDOWS 1
#endif

//For resources of type A with paths formatted as "[B][resource name][C]", creates as constants the start- and ending strings and their lengths, to be used to build the full file path later.
#define PATH(A,B,C) const char* A##_path_start = B; const int A##_path_start_len = sizeof(B)-1; const char* A##_path_end = C; const int A##_path_end_len = sizeof(C)-1
//For resources of type A with two relevant paths, e.g. an image file and a data file, formatted as "[B][resource name][C]" and "[B][resource name][D]" respectively.
#define F_PATH(A,B,C,D) const char* A##_path_start = B; const int A##_path_start_len = sizeof(B)-1; const char* A##_path_end = C; const int A##_path_end_len = sizeof(C)-1; const char* A##_data_end = D; const int A##_data_end_len = sizeof(D)-1


//Data formats
#define IMG_FORMAT ".png"
#define MUS_FORMAT ".wav"
#define DAT_FORMAT ".dat"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Windows-style paths
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#if WINDOWS == 1
//Data sources
const char* script_raw = "Scripts\\script_raw.txt";
const char* loc_raw    = "Scripts\\loc_raw.txt";
const char* dat_raw    = "Scripts\\dat_raw.txt";

const char* script_bin = "Scripts\\scripts.dat";
const char* loc_bin    = "Scripts\\loc.dat";
const char* dat_bin    = "Scripts\\names.dat";

//Config data
const char* cfg_source = "config.txt";

//Static asset locations
//Sky
#define SKY_PATH(name)  "IMG\\BG\\sky\\" #name IMG_FORMAT

//Dynamic asset locations
//Background
PATH(loc,"IMG\\BG\\","\\main" IMG_FORMAT);
const char*   shd_path_end =        "\\shade"     IMG_FORMAT;
const int shd_path_end_len = sizeof("\\shade"     IMG_FORMAT);
const char*   eve_path_end =        "\\shade_eve" IMG_FORMAT;
const int eve_path_end_len = sizeof("\\shade_eve" IMG_FORMAT);
const char*   ngt_path_end =        "\\shade_ngt" IMG_FORMAT;
const int ngt_path_end_len = sizeof("\\shade_ngt" IMG_FORMAT);
//Music
PATH(mus,"MUS\\BGM\\",MUS_FORMAT);
//Sound Effects
PATH(sef,"MUS\\SE\\",MUS_FORMAT);
//Scenes
PATH(scn,"IMG\\SCN\\",IMG_FORMAT);
//Fonts
F_PATH(fnt,"Fonts\\",IMG_FORMAT,DAT_FORMAT);


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Mac/Linux paths
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#else
//Data sources
const char* script_raw = "Scripts/script_raw.txt";
const char* loc_raw    = "Scripts/loc_raw.txt";
const char* dat_raw    = "Scripts/dat_raw.txt";

const char* script_bin = "Scripts/scripts.dat";
const char* loc_bin    = "Scripts/loc.dat";
const char* dat_bin    = "Scripts/names.dat";

//Config data
const char* cfg_source = "config.txt";

//Static asset locations
//Sky
#define SKY_PATH(name)  "IMG/BG/sky/" #name IMG_FORMAT

//Dynamic asset locations
//Background
PATH(loc,"IMG/BG/","/main" IMG_FORMAT);
const char*   shd_path_end =        "/shade"     IMG_FORMAT;
const int shd_path_end_len = sizeof("/shade"     IMG_FORMAT);
const char*   eve_path_end =        "/shade_eve" IMG_FORMAT;
const int eve_path_end_len = sizeof("/shade_eve" IMG_FORMAT);
const char*   ngt_path_end =        "/shade_ngt" IMG_FORMAT;
const int ngt_path_end_len = sizeof("/shade_ngt" IMG_FORMAT);
//Music
PATH(mus,"MUS/BGM/",MUS_FORMAT);
//Sound Effects
PATH(sef,"MUS/SE/",MUS_FORMAT);
//Scenes
PATH(scn,"IMG/SCN/",IMG_FORMAT);
//Fonts
F_PATH(fnt,"Fonts/",IMG_FORMAT,DAT_FORMAT);
#endif


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//Assorted helper macros:
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Writes a 2-byte integer A to file B.
#define writeInt(A,B) fputc((A/256),B); fputc((A%256),B)

//Reads a 2-byte integer from file B, without checking for EOF (If at EOF, should result in the integer becoming negative).
#define readInt(B) fgetc(B)*256+fgetc(B)

//Builds a path for a resource. A: code for type of resource; B: name of resource.
#define buildPath(A,B) \
	memcpy(A##_paths[B],A##_path_start,A##_path_start_len);\
	memcpy(A##_paths[B]+A##_path_start_len,A##_names[B],A##_name_lengths[B]);\
	memcpy(A##_paths[B]+A##_path_start_len+A##_name_lengths[B],A##_path_end,A##_path_end_len);\
	DBG(5,"Registered %s path '%s'",type_names[type_##A],A##_paths[B])
	
#define buildDataPath(A,B) \
	memcpy(A##_data_paths[B],A##_path_start,A##_path_start_len);\
	memcpy(A##_data_paths[B]+A##_path_start_len,A##_names[B],A##_name_lengths[B]);\
	memcpy(A##_data_paths[B]+A##_path_start_len+A##_name_lengths[B],A##_data_end,A##_data_end_len);\
	DBG(5,"Registered %s data path '%s'",type_names[type_##A],A##_paths[B])

#define buildShadePath(A,B,C) \
	memcpy(A,loc_path_start,loc_path_start_len);\
	memcpy(A+loc_path_start_len,loc_names[B],loc_name_lengths[B]);\
	memcpy(A+loc_path_start_len+loc_name_lengths[B],C##_path_end,C##_path_end_len);\
	DBG(5,"Built shade path '%s'",A)
	