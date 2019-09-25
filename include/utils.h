#ifndef _UTILS_
#define _UTILS_
#include "headerDependencies.h"
char * m_shell_read_line();
int give_count_of_pipes(char * command);
int determine_case(char * command);
int stringToInt(char * str);
int is_it_background(char * command);
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

#endif