#ifndef _CALL_BUILTIN_
#define _CALL_BUILTIN_
/*
Calls the appropriate builtin function passing it the options and arguments
@param[in]		builtin_index 	index of the builtin command to run
@param[in]		options_array	contains the options and arguments for the commands
@param[in]		number_of_args	number of args present in the options array
*/
void call_appropriate_function(int builtin_index,char ** options_array,int number_of_args);
#endif