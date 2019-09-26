#ifndef _CD_
#define _CD_
#include "headerDependencies.h"

int print_error_for_cd();
/*
Similar to cd command
@param[in] 		current_path  		path of the current directory
@param[in] 		arguments	 		directory to which user want to move
@return 		0 if successful ,-1 on error
*/
int m_cd(char* current_path,char ** arguments,int number_of_args);
#endif