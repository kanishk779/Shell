#include "parser.h"
#include "headerDependencies.h"
#include "utils.h"
#include "main.h"
#include "builtin.h"
#include "history.h"
#include "execute.h"
/*
strtok() will remember which string it operated on last time even if a function 
call happen.
*/
char ** fill_argument_array(int words)
{
	char ** options = (char **)malloc((words)*sizeof(char *));
	int index = 0;
	char *command_option = strtok(NULL,MAIN_TOK_DELIM);
	while(command_option != NULL)
	{
		options[index] = command_option;
		index++;
		command_option = strtok(NULL,MAIN_TOK_DELIM);
	}
	// execvp needs the argument list to end with a null pointer.
	options[index]=NULL;
	return options;
}

int parse_up_command(char* command)
{
	int len = strlen(command);
	int is_up_arrow = true;
	int up_count = 0;
	for(int i=0;i<len;i++)
	{
		if (command[i] == '\033') { // if the first value is esc
	    i+=2; // skip the [
	    switch(command[i]) { // the real value
	        case 'A':
	        	up_count++;
	            break;
	        case 'B':
	        	is_up_arrow = false;
	            break;
	        case 'C':
	        	is_up_arrow = false;
	            break;
	        case 'D':
	        	is_up_arrow = false;
	            break;
	    	}
		}
		else
		{
			is_up_arrow = false;
			break;
		}
	}
	if(!is_up_arrow)
		return 0;
	char * kth_command = get_kth_command(up_count);
	if(kth_command == NULL)
		return 1;
	parse_individual_command(kth_command);
	return 1;
}
void parse_commands(char* commands)
{
	int num_commands = number_of_commands_separated_by_semicolon(commands);
	char ** commands_array = (char **)malloc(num_commands*sizeof(char *));
	// first separate on the basis of the ';'
	char *command_fetcher = strtok(commands,COMMAND_DELIM);
	int index = 0;
	while(command_fetcher != NULL)
	{
		commands_array[index] = command_fetcher;
		index++;
		command_fetcher = strtok(NULL,COMMAND_DELIM);
	}

	// now parse individual commands
	int parsed_com = 0;
	while(parsed_com < index)
	{
		int result = parse_individual_command(commands_array[parsed_com]);
		if(result == 1)
		{
			printf("error in parsing %s\n",command_fetcher);
			exit(1);
		}
		parsed_com++;
	}
	free(commands_array);
}
int parse_individual_command(char* command)
{
	// The function will only work if up command is given
	if(parse_up_command(command))
		return 0;
	//
	int pipe_count = give_count_of_pipes(command);
	int redirection_present = determine_case(command);
	// here check if it not a builtin command
	
	int words = count_of_words_in_str(command);
	char * builtin_helper = (char *)malloc(512);
	strcpy(builtin_helper , command);
	char * echo_helper = (char *) malloc(512);
	strcpy(echo_helper,command);
	char * command_option = strtok(builtin_helper,MAIN_TOK_DELIM);
	// first check if it is a builtin or not
	int is_builtin = false;
	int builtin_index=-1;
	for(int i=0;i<NO_OF_BUILTIN;i++)
	{
		if(strcmp(command_option,builtins[i]) == 0)
		{
			// we have found that it is a builtin command
			is_builtin = true;
			builtin_index = i;
			break;
		}
	}
	if(pipe_count >= 1 || redirection_present || (is_builtin == false))
	{
		//separate on the basis of Pipe
		// create a job here
		job* new_job = init_job();
		int len = strlen(command);
		char * name_of_job = (char *)malloc(len+2);
		strcpy(name_of_job,command);
		new_job->job_name = name_of_job;
		int back = is_it_background(command);
		int pipe_count = give_count_of_pipes(command);
		char ** arr = (char **)malloc((pipe_count+1)*sizeof(char *));
		char * separate = strtok(command,PIPE_DELIM);
		int idx = 0;
		while(separate != NULL)
		{
			int len = strlen(separate);
			arr[idx] = (char *)malloc(len+1);
			arr[idx] = separate;
			separate = strtok(NULL,PIPE_DELIM);
			idx++;
		}
		
	  	for(int i=0;i<=pipe_count;i++)
	  	{
	  		// first tokenize the string;
	  		int var = determine_case(arr[i]);
	  		int words = count_of_words_in_str(arr[i]);
	  		char ** words_arr = (char **) malloc((words) * sizeof(char*));
	  		char * separate = strtok(arr[i],MAIN_TOK_DELIM);
	  		int idx = 0;
	  		while(separate != NULL)
	  		{
	  			int len = strlen(separate);
	  			words_arr[idx] = (char * )malloc(len+1);
	  			words_arr[idx] = separate;
	  			separate = strtok(NULL,MAIN_TOK_DELIM);
	  			idx++;
	  		}
	  		
	  		struct command_structure *sub_command;
	  		sub_command = init_command_structure();

	  		sub_command->variant = var;
	  		char ** command = (char **)malloc((words+1) * sizeof(char *));
	  		int index = 0;
	  		for(int j=0;j<words;j++)
	  		{
	  			if(strcmp(words_arr[j],"<") == 0)
	  			{
	  				char * input;
	  				input = (char *)malloc(strlen(words_arr[j+1])+1);
	  				input = words_arr[j+1];
	  				sub_command->input_file = input;
	  				j++;
	  			}
	  			else if((strcmp(words_arr[j],">") == 0) || (strcmp(words_arr[j],">>") == 0))
	  			{
	  				char * output;
	  				output = (char *)malloc(strlen(words_arr[j+1])+1);
	  				output = words_arr[j+1];
	  				sub_command->output_file = output;
	  				j++;
	  			}
	  			else if(strcmp(words_arr[j],"&") == 0)
	  			{
	  				// just skip it
	  			}
	  			else
	  			{
	  				command[index] = words_arr[j];
	  				index++;
	  			}
	  		}
	  		command[index] = NULL;
	  		sub_command->argv = command;
	  		//insert_command_structure(sub_command,new_job);
	  		if(new_job->first_command == NULL)
	  		{
	  			new_job->first_command = sub_command;
	  		}
	  		else
	  		{
	  			command_structure * temp = new_job->first_command;
	  			while(temp->next != NULL)
	  				temp = temp->next;
	  			temp->next = sub_command;
	  		}
	  		
	  	}
	  	// insert the job into job list
	  	if(!back)
	  		new_job->foreground = true;
	  	//insert_job(new_job);
	  	if(first_job == NULL)
	  	{
	  		first_job = new_job;
	  	}
	  	else
	  	{
	  		job * temp = first_job;
	  		while(temp->next != NULL)
	  			temp = temp->next;
	  		temp->next = new_job;
	  	}
	  	fork_pipes(pipe_count,new_job);
		return 0;
	}	
	else if(builtin_index==12)
	{
		int idx = 0;
		char **new_arg_list = (char **)malloc((words-3)*sizeof(char*));
		char * separate = strtok(NULL,MAIN_TOK_DELIM);
		int command_started = false;
		int command_stopped = false;
		char * prev;
		int t,p;
		while(separate != NULL)
		{
			if(strcmp(separate,"-c") == 0)
			{
				command_started = true;
				separate = strtok(NULL,MAIN_TOK_DELIM);
				continue;
			}
			if(strcmp(separate , "-t") == 0)
			{
				command_started = false;
				command_stopped = true;
				prev = separate;
				separate = strtok(NULL,MAIN_TOK_DELIM);
				continue;
			}
			if(strcmp(separate , "-p") == 0)
			{
				prev = separate;
			}
			if(command_stopped && (strcmp(prev,"-t") == 0))
			{
				t = stringToInt(separate);
			}
			if(command_stopped && (strcmp(prev,"-p") == 0))
			{
				p = stringToInt(separate);
			}
			if(command_started)
			{
				int len = strlen(separate);
				new_arg_list[idx] = (char *)malloc(len+1);
				new_arg_list[idx] = separate;
				idx++;
			}
			separate = strtok(NULL,MAIN_TOK_DELIM);
		}
		new_arg_list[idx]=NULL;
		cronjob(new_arg_list,t,p);
	}
	else if(builtin_index==2)
	{
		// search for 'e'
		char c;
		c = echo_helper[0];
		int ind = 0;
		while(c != 'e')
		{
			ind++;
			c = echo_helper[ind];
		}
		int new_ind = ind+4;
		c = echo_helper[new_ind];
		// now continue till you find a character different from ' '
		while(c == ' ')
		{
			new_ind++;
			c = echo_helper[new_ind];
		}
		int status = m_echo(echo_helper+new_ind);
	}
	else
	{
		char ** arg_array = fill_argument_array(words);
		if(words>=2 && arg_array[words-2]!=NULL && strcmp(arg_array[words - 2],"&") == 0)
		{
			// this is a back-ground process
			call_appropriate_function(builtin_index,arg_array,words-2);
		}
		else
			call_appropriate_function(builtin_index,arg_array,words - 1);
	}
	return 0;
}
