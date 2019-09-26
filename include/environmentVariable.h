#ifndef _ENVIRONMENT_VARIABLE_
#define _ENVIRONMENT_VARIABLE_
#include "headerDependencies.h"
/*
Sets the value of the environment variable
@param[in]		arguments		contains the  name and value of variable
@param[in]		number_of_args
*/
int m_setenv(char ** arguments,int number_of_args);

/*
Sets the value of the environment variable
@param[in]		arguments		contains the name of the variable
@param[in]		number_of_args 	
*/
int m_unsetenv(char ** arguments,int number_of_args);
#endif