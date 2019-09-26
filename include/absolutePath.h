#ifndef _ABSOLUTE_PATH_
#define _ABSOLUTE_PATH_
#include "headerDependencies.h"

char* giveAbsolutePath(char *path);

/*
gives the relative path in-case the current directory is inside
the the home-directory(where this project resides).
gives absolute path in-case the current directory is the ancestor
of the home-directory.
@param[in]		curr_dir	current directory in shell
@param[in]		home_dir 	home directory determined in the
							beginning of the program.
@return 		the absolute or relative path.
*/ 
char * give_relative_or_absolute_path(char * curr_dir,char* home_dir);

#endif