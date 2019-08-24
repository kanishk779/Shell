#ifndef _MAIN_H_
#define _MAIN_H_
#include "headerDependencies.h"
#define clear() printf("\033[H\033[J")
#define MAIN_TOK_DELIM " \t\r\n\a"
#define COMMAND_DELIM ";"
#define NO_OF_BUILTIN 10
#define HOST_NAME_MAXIMUM 100
#define LOGIN_NAME_MAXIMUM 100
#define MAX_COMMAND_LEN 500
#define QUEUE_SIZE 20
#define PATH_MAXIMUM 1024

/*
This function displays the initial information to the user,
when starting the shell. It also initialises the variables.
*/
void init_shell();

/*
Shows the <username@system_name:curr_dir> prompt to the user 
after executing every command.
*/
void show_user_sys_name();

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
/*
Parse the semi-colon separated list of commands
@param[in] 		commands 	pointer to command which this method parses
@return 		a string of command and its options. (don't know will use it)
*/
void parse_commands(char* commands);

/*
Parse the individual command sent by the parse_commands function
@param[in]		command 	pointer to command which this method parses
@return 		status of the execution of the command 0->successful 1->failure
*/
int parse_individual_command(char* command);

/*
*/
char ** fill_argument_array(int words);
/*
gives the number of the words in the string given
@param[in]		str 		string in which the number of words are to returned
@return 		the number of words str
*/
int count_of_words_in_str(char * str);

/*
Gives the number of commands separated by semicolon
@param[in]		str 		string for which no. of commands are to be returned
@return 		the number of commands
*/
int number_of_commands_separated_by_semicolon(char *str);

/*
Calls the appropriate builtin function passing it the options and arguments
@param[in]		builtin_index 	index of the builtin command to run
@param[in]		options_array	contains the options and arguments for the commands
@param[in]		number_of_args	number of args present in the options array
*/
void call_appropriate_function(int builtin_index,char ** options_array,int number_of_args);

/*
Free all the heap allocated memory so that no memory leaks take 
place once the program is over.
*/
void m_free();

/*
This function displays the various builtin command that this
shell supports.
@param[in]		file 		the file pointer to which the information
							is to be displayed.
*/
void m_help(FILE * file);

/*
gives the length of the string (in case strlen doesn't work)
@param[in]		str 		the string whose length is calculated
@return 		returns the length of the string
*/
int m_strlen(char *str);

/*
reads a line of command from the terminal
*/
char * m_shell_read_line();
#endif

	