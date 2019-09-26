#ifndef _PARSER_
#define _PARSER_
/*
parse the up command for running the previous commands
*/
int parse_up_command(char* command);

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
creates the arguments list for command to run in execvp
*/
char ** fill_argument_array(int words);
#endif