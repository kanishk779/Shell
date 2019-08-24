#include "headerDependencies.h"
#include "main.h"
#include "builtin.h"
char* home_directory;
char hostname[HOST_NAME_MAXIMUM]; // also called as system/computer name
char *username;
char *builtins[NO_OF_BUILTIN];
int rear,front,size;
char **arr;
int used[QUEUE_SIZE];
pid_t foreground_p;
int loop_Stop;
//pid_t background_processes_pid[100];
void handler()
{
	int status;
	pid_t w = waitpid(-1,&status,WNOHANG);
	if(w == -1)
	{
		perror("There was error in wait_pid in handler\n");
		exit(0);
	}
	if(WEXITSTATUS(status))
		printf("The process with pid - %u exited \n",w);
	else if(WIFSIGNALED(status))
		printf("The process with pid - %u was signalled \n",w);
}
char* remove_from_Queue()
{
    if (front == -1) 
    { 
        printf("\nNO commands have been given till now\n"); 
        return NULL; 
    } 
  
    char* data = arr[front]; 
    if (front == rear) 
    { 
       	front = -1; 
       	rear = -1; 
    } 
    else if (front == size-1) 
       	front = 0; 
    else
        front++; 
    return data; 
}
void fill_Queue(char * str)
{
    if ((front == 0 && rear == size-1) || (rear == (front-1)%(size-1))) 
    { 
     	// we first need to deque and then enque
        remove_from_Queue();
        fill_Queue(str); 
        return; 
    }
    else if (front == -1)
    { 
       	front = rear = 0;
       	int len = strlen(str);
       	if(used[rear])
       	arr[rear] = (char *)realloc(arr[rear],len+1);
       	else
       	arr[rear] = (char *)malloc(len+1); 
       	strcpy(arr[rear],str);
       	used[rear] = 1;
   	} 
    else if (rear == size-1 && front != 0) 
    { 
       	rear = 0; 
       	int len = strlen(str);
       	if(used[rear])
       	arr[rear] = (char *)realloc(arr[rear],len+1);
       	else
       	arr[rear] = (char *)malloc(len+1);
       	strcpy(arr[rear],str);
       	used[rear] = 1; 
   	} 
    else
    {
       	rear++; 
       	int len = strlen(str);
       	if(used[rear])
       	arr[rear] = (char *)realloc(arr[rear],len+1);
       	else
       	arr[rear] = (char *)malloc(len+1);
       	strcpy(arr[rear],str);
       	used[rear] = 1; 
    }
}
int main()
{
	// initialise shell and its variables
	init_shell();
	//signal(SIGCHLD,handler);
	int loop = true;
	char *line;
	while(loop)
	{
		show_user_sys_name();
		line = m_shell_read_line();
		fill_Queue(line);
		int len = strlen(line);
		line[len-1]='\0';
		parse_commands(line);
		free(line);
	}
	return 0;
}
void init_shell()
{
	clear();
    size = QUEUE_SIZE;
	rear = front = -1;
	arr = (char **)malloc(QUEUE_SIZE * (sizeof(char *)));
	home_directory = (char *)malloc(PATH_MAXIMUM * sizeof(char));
	if(getcwd(home_directory,PATH_MAXIMUM) == NULL)
	{
		perror("could not get the home directory\n");
		exit(1);
	}
	username = (char *)malloc(LOGIN_NAME_MAXIMUM * sizeof(char));
	gethostname(hostname,HOST_NAME_MAXIMUM);
	username = getlogin();
	if(username == NULL)
	{
		perror("could get username\n");
		exit(1);
	}
	builtins[0] = "ls";
	builtins[1] = "cd";
	builtins[2] = "echo";
	builtins[3] = "exit";
	builtins[4] = "history";
	builtins[5] = "help";
	builtins[6] = "clear";
	builtins[7] = "pinfo";
	builtins[8] = "nightswatch";
	builtins[9] = "pwd";

	printf("------------------------This is shell inside shell ----------------\n");
}

// working fine
char * m_shell_read_line()
{
	char *line = NULL;
	size_t buffer_size = 0;
	getline(&line,&buffer_size,stdin);
	return line;
}
void show_user_sys_name()
{
	char * current_directory;
	current_directory = (char *)malloc(PATH_MAXIMUM * sizeof(char));
	if(getcwd(current_directory,PATH_MAXIMUM) != NULL)
	{
		char *relative_dir = give_relative_or_absolute_path(current_directory,home_directory);
		printf("%s@%s:%s>", username,hostname,relative_dir);
	}
	else
	{
		perror("could not get the current_directory\n");
		exit(1);
	}
	free(current_directory);
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
// it is working
int count_of_words_in_str(char * str)
{
	int len = strlen(str);
	char prev = str[0];
	int words = 0;
	if(str[len-1] != ' ')
		words++;
	for(int i=1;i<len;i++)
	{
		if(prev!=' ' && str[i]==' ')
			words++;
	}
	return words;
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
int parse_individual_command(char* command)
{
	int words = count_of_words_in_str(command);
	char * echo_helper = (char *) malloc(512);
	strcpy(echo_helper,command);
	char * command_option = strtok(command,MAIN_TOK_DELIM);
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
	if(is_builtin && builtin_index==2)
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
		if(is_builtin)
		{
			if(words>=2 && arg_array[words-2]!=NULL && strcmp(arg_array[words - 2],"&") == 0)
			{
				// this is a back-ground process
				call_appropriate_function(builtin_index,arg_array,words-2);
			}
			else
				call_appropriate_function(builtin_index,arg_array,words - 1);
		}
		else
		{
			// now we need to do fork and then use execvp
			// first find whether the command is foreground or background
			int is_background = false;
			char **new_arg_list;
			if(words>=2 && arg_array[words-2]!=NULL && strcmp(arg_array[words - 2],"&") == 0)
			{
				is_background = true;
				int idx = 0;
				
				new_arg_list = (char **)malloc((words+1)*sizeof(char*));
				new_arg_list[idx] = command_option;
				idx++;
				int arg_array_idx = 0;
				for(;idx<=words-2;idx++)
				{
					new_arg_list[idx] = arg_array[arg_array_idx];
					arg_array_idx++;
				}
				new_arg_list[idx]=NULL;
			}
			else
			{
				int idx=0;
				new_arg_list = (char **)malloc((words+1)*sizeof(char*));
				new_arg_list[idx] = command_option;
				idx++;
				int arg_array_idx = 0;	
				for(;idx<=words;idx++)
				{
					new_arg_list[idx] = arg_array[arg_array_idx];
					arg_array_idx++;
				}
			}
			pid_t pid_returned = fork();
			if(pid_returned == -1)
			{
				perror("fork error\n");
				exit(0);
			}
			if(pid_returned == 0)
			{
				pid_t pgrpid = getpgrp();
				if((tcsetpgrp(2,pgrpid))<0)
				{
					perror("error in starting the foreground process\n");
				}
				if(is_background)
					setpgid(0,0);
				int status = execvp(new_arg_list[0],new_arg_list);
			}
			else
			{
				int status;
				if(!is_background)
				{
					int status;
					pid_t pd = waitpid(pid_returned,&status,0);
					
					if(WEXITSTATUS(status))
						printf("The process with pid - %u exited \n",pid_returned);
					else if(WIFSIGNALED(status))
						printf("The process with pid - %u was signalled \n",pid_returned);
				}
				else
				{
					// maybe use the handler
					printf("[proc %d started]\n", pid_returned);
					signal(SIGCHLD,handler);
				}
			}
		}
	}
	return 0;
}
void call_appropriate_function(int builtin_index,char ** options_array,int number_of_arguments)
{
	char* curr_directory = (char *)malloc(PATH_MAXIMUM * sizeof(char));
	if(getcwd(curr_directory,PATH_MAXIMUM)==NULL)
	{
		perror("error in getting curr_directory when calling builtin\n");
		exit(1);
	}
	int status;
	switch(builtin_index)
	{
		case 0:
			status = m_ls(curr_directory,options_array,number_of_arguments);
			if(status == -1)
			{
				perror("ls function was not able to get executed\n");
			}
			break;
		case 1:
			status = m_cd(curr_directory,options_array,number_of_arguments);
			if(status == -1)
			{
				perror("cd function was not able to get executed\n");
			}
			break;
		case 3:
			m_exit();
			break;
		case 4:
			if(number_of_arguments == 0)
				history(10);
			else
			{
				int len = strlen(options_array[0]);
				int power = len - 1;
				int number_of_commands=0;
				for(int i=0;i<len;i++)
				{
					number_of_commands += (options_array[0][i]-'0')*pow(10,power);
					power--;
				}
				history(number_of_commands);
			}	
			break;
		case 5:
			m_help(NULL);
			break;
		case 6:
			clear();
			break;
		case 7:
			p_info(options_array,number_of_arguments);
			break;
		case 8:
			nightswatch(options_array,number_of_arguments);
			break;
		case 9:
			{
				char * current_directory;
				current_directory = (char *)malloc(PATH_MAXIMUM * sizeof(char));
				if(getcwd(current_directory,PATH_MAXIMUM) != NULL)
				{
					pwd(current_directory);
					free(current_directory);
				}
				else
					printf("Error in getting the curr_directory\n");
				break;
			}
			
		default:
			perror("Are you sane??\n");
	}
	free(curr_directory);
}
char * give_relative_or_absolute_path(char * curr_dir,char* home_dir)
{
	int curr_dir_len = strlen(curr_dir);
	int home_dir_len = strlen(home_dir);
	int curr_dir_ind = 0;
	int not_found = false;
	if(curr_dir_len < home_dir_len)
	{
		return curr_dir;
		 
	}
	for(int i=0;i<home_dir_len;i++)
	{
		if(home_dir[i] != curr_dir[curr_dir_ind])
		{
			not_found = true;
			break;
		}
		curr_dir_ind++;
	}
	if(not_found)
		return curr_dir;
	else if(curr_dir_len == home_dir_len)
	{
		char *str = (char *)malloc(4);
		strcpy(str,"~");
		return str;
	}
	else
	{
		char * str = (char* )malloc(curr_dir_len - home_dir_len + 3);
		int j = home_dir_len+1;
		for(int i=0;i<curr_dir_len-home_dir_len;i++)
		{
			str[i] = curr_dir[j];
			j++;
		}
		return str;
	}
	return curr_dir;
}
