#ifndef _PARSER_
#define _PARSER_

char ** fill_argument_array(int words);
int parse_up_command(char* command);
void parse_commands(char* commands);
int parse_individual_command(char* command);

#endif