#ifndef _BUILTIN_H_
#define _BUILTIN_H_
#include"headerDependencies.h"
#include"main.h"
#define ANSI_COLOR_GRN   "\x1B[32m"
#define ANSI_COLOR_BLU   "\x1B[34m"
#define ANSI_COLOR_MAG   "\x1B[35m"
#define ANSI_COLOR_RESET "\x1B[0m"
#define PATH_M 1024
/*
Similar to ls command
@param[in] 		current_path			path of the current directory
@return 		0 if succesful , -1 on error
TODO: 
	- how will the options be handled
*/
int m_ls(char* current_path,char ** options,int number_of_args);

/*
Similar to cd command
@param[in] 		current_path  		path of the current directory
@param[in] 		arguments	 		directory to which user want to move
@return 		0 if successful ,-1 on error
*/
int m_cd(char* current_path,char ** arguments,int nunber_of_args);

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
A modified version of watch command (supports )
@param[in]		arguments		options for the command and type of command
@param[in]		number_of_args	number of char pointers in arguments
@return 		0 on success , -1 on failure
*/
int nightswatch(char ** arguments, int number_of_args);

/*
Shows the relevant information for helping shell user
@param[in]		file 			file to which the help information has to be
								written
*/
void m_help(FILE * file);

/*
Shows the history of the commands given to the shell
@param[in]		how_many		number of commands to show
*/
void history(int how_many);

/*
Shows info about the process
@param[in] 		arguments		contains the pid of the process if given
@param[in]		number_of_args 
*/
int p_info(char ** arguments,int number_of_args);

#endif
