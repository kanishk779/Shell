#ifndef _BUILTIN_H_
#define _BUILTIN_H_
#include"headerDependencies.h"
#include"main.h"
#include "absolutePath.h"


/*
Similar to echo command
@param[in] 		user_msg 		message that the user want to print
@return 		0 if successful ,-1 on error
*/
int m_echo(char* user_msg);

/*
Use to exit the shell program
*/
void m_exit();

/*
Prints the current working directory
*/
void pwd(char * path);

/*
Shows the relevant information for helping shell user
@param[in]		file 			file to which the help information has to be
								written
*/
void m_help(FILE * file);

/*
Shows info about the process
@param[in] 		arguments		contains the pid of the process if given
@param[in]		number_of_args 
*/
int p_info(char ** arguments,int number_of_args);

/*
Runs a commands repeatedly over a period 'p' after every 't' seconds
@param[in]		command 		the command that needs to be executed
@param[in]		p 				the total time period given
@param[in]		t 				the time between successive runs of the command
*/
void cronjob(char ** command, int t ,int p);
#endif
