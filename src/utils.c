#include "utils.h"

char * m_shell_read_line()
{
	char *line = NULL;
	size_t buffer_size = 0;
	if (getline(&line,&buffer_size,stdin) < 0) {
		perror("getline");
		exit(1);
	}
	return line;
}
int count_of_words_in_str(char * str)
{
	int len = strlen(str);
	char prev = str[0];
	int words = 0;
	for(int i=1;i<len;i++)
	{
		if(prev!=' ' && str[i]==' ')
			words++;
		prev = str[i];
	}
	if(str[len-1] != ' ')
		words++;
	return words;
}
int number_of_commands_separated_by_semicolon(char *str)
{
	int len = strlen(str);
	int commands=0;
	for(int i=0;i<len;i++)
	{
		if(str[i] == ';')
			commands++;
	}
	for(int i=len-1;i>=0;i++)
	{
		if(str[i] == ';')
			break;
		else if(str[i] != ' ')
		{
			commands++;
			break;
		}
	}
	return commands;
}

int give_count_of_pipes(char * command)
{
	int count = 0;
	int len = strlen(command);
	for(int i=0;i<len;i++)
	{
		if(command[i] == '|')
			count++;
	}
	return count;
}
int determine_case(char * command)
{
	int len = strlen(command);
	int left_found = false;
	int right_found = false;
	int right_append_found = false;
	for(int i=0;i<len;i++)
	{
		if(command[i] == '<')
			left_found = true;
		if(command[i] == '>' && (i+1)<len && command[i+1]!='>')
			right_found = true;
		if(command[i] == '>' && (i+1)<len && command[i+1]=='>')
			right_append_found = true;
	}
	if(left_found && right_append_found)
		return 5;
	if(right_append_found)
		return 4;
	if(left_found && right_found)
		return 3;
	if(left_found)
		return 1;
	if(right_found)
		return 2; 
		
	return 0;
}
int stringToInt(char * str)
{
	int len = strlen(str);
	int result = 0;
	for(int i=0;i<len;i++)
	{
		result = result*10 + (str[i] - '0');
	}
	return result;
}
int is_it_background(char * command)
{
	int len = strlen(command);
	int background = false;
	for(int i=0;i<len;i++)
	{
		if(command[i] == '&')
			background = true;
	}
	return background;
}