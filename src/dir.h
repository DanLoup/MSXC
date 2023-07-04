#ifdef WIN32
    #include <direct.h>
    #define GetCurrentDir _getcwd
	#define SetCurrentDir _chdir
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
	#define SetCurrentDir chdir
#endif
