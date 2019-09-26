#ifndef _LS_
#define _LS_
#include "headerDependencies.h"
#define PATH_M 1024
#define ANSI_COLOR_GRN   "\x1B[32m"
#define ANSI_COLOR_BLU   "\x1B[34m"
#define ANSI_COLOR_MAG   "\x1B[35m"
#define ANSI_COLOR_RESET "\x1B[0m"


void printMonthDayYear(struct stat fileStats);
void printFileType(struct stat fileStats);
void printPermissions(struct stat fileStats);
void printFileNames(char *name,struct stat *fileStats);
void print_info(DIR * mydir,char * given_directory,struct dirent *myfile,int is_l_mentioned,int is_a_mentioned);
/*
Similar to ls command
@param[in] 		current_path			path of the current directory
@return 		0 if succesful , -1 on error
TODO: 
	- how will the options be handled
*/
int m_ls(char* current_path,char ** options,int number_of_args);
#endif 