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
pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
job * first_job;
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
char * get_kth_command(int k)
{
	int count = 1;
	int rear_temp = rear;
	while(rear_temp != front)
	{
		if(count == k)
		{
			return arr[rear_temp];
		}
		else
		{
			rear_temp = (rear_temp - 1 + QUEUE_SIZE)%QUEUE_SIZE;
			count++;
		}
	}
	printf("User has not given that many commands till now\n");
	return NULL;
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
		int len = strlen(line);
		line[len-1]='\0';
		fill_Queue(line);
		parse_commands(line);
		free(line);
	}
	return 0;
}

job * init_job()
{
	job * new_job = (job *)malloc(sizeof(job));
	new_job->next = NULL;
	new_job->command = NULL;
	new_job->first_command = NULL;
	new_job->pgid = -1;
	new_job->notified = 0;
	return new_job;
}
command_structure * init_command_structure()
{
	command_structure * command = (command_structure *)malloc(sizeof(command_structure));
	command->next = NULL;
	command->input_file = NULL;
	command->output_file = NULL;
	command->stopped = 0;
	command->completed = 0;
	command->status = 0;
	command->variant = -1;
	command->pid = -1;
	return command;
}
void init_shell()
{
	clear();
	shell_terminal = STDIN_FILENO;

	while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp ()))
        kill (shell_pgid, SIGTTIN);

    /* Ignore interactive and job-control signals.  */
    signal (SIGINT, SIG_IGN);
    signal (SIGQUIT, SIG_IGN);
    signal (SIGTSTP, SIG_IGN);
    signal (SIGTTIN, SIG_IGN);
    signal (SIGTTOU, SIG_IGN);
    signal (SIGCHLD, SIG_IGN);

      /* Put ourselves in our own process group.  */
    shell_pgid = getpid ();
    if (setpgid (shell_pgid, shell_pgid) < 0)
    {
        perror ("Couldn't put the shell in its own process group");
        exit (1);
    }

    /* Grab control of the terminal.  */
    tcsetpgrp (shell_terminal, shell_pgid);

    /* Save default terminal attributes for shell.  */
    tcgetattr (shell_terminal, &shell_tmodes);
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
	builtins[3] = "quit";
	builtins[4] = "history";
	builtins[5] = "help";
	builtins[6] = "clear";
	builtins[7] = "pinfo";
	builtins[8] = "nightswatch";
	builtins[9] = "pwd";
	builtins[10]= "setenv";
	builtins[11]= "unsetenv";
	builtins[12]= "cronjob";

	printf("------------------------This is shell inside shell ----------------\n");
}

// working fine
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

job *find_job (pid_t pgid)
{
  	job *j;

  	for (j = first_job; j; j = j->next)
    if (j->pgid == pgid)
      	return j;
  	return NULL;
}

/* Return true if all processes in the job have stopped or completed.  */
int job_is_stopped (job *j)
{
  	command_structure *p;

  	for (p = j->first_command; p; p = p->next)
    	if (!p->completed && !p->stopped)
      		return 0;
  	return 1;
}

/* Return true if all processes in the job have completed.  */
int job_is_completed (job *j)
{
  	command_structure *p;

  	for (p = j->first_command; p; p = p->next)
    	if (!p->completed)
      	return 0;
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
int spawn_proc (int in, int out, struct command_structure *cmd) {
	// 5 cases can occur
	// case 0 -> no redirection is present
	// case 1 -> cmd < input
	// case 2 -> cmd arguments > somefile
	// case 3 -> cmd < input > somefile
	// case 4 -> cmd arguments >> somefile
	// case 5 -> cmd < input >> somefile
    pid_t pid;
    if ((pid = fork ()) == 0) {
    	
        if (in != 0) {
            dup2 (in, 0);
            close (in);
        }
        if (out != 1) {
            dup2 (out, 1);
            close (out);
        }
        
        switch(cmd->variant)
        {
        	case 0:
        	{
        		int return_code = execvp(cmd->argv [0], (char **)cmd->argv);
		        if (return_code < 0) {
		            perror("execvp failed");
		            exit(1);
		        }
        		break;
        	}
        	case 1:
        	{
        		int input_fd = open(cmd->input_file,O_RDONLY,0);
        		dup2(input_fd,0);
        		int return_code = execvp(cmd->argv [0], (char **)cmd->argv);
		        if (return_code < 0) {
		            perror("execvp failed");
		            exit(1);
		        }
		        close(input_fd);
        		break;
        	}
        	case 2:
        	{
        		int output_fd = open(cmd->output_file, O_RDWR | O_CREAT, 0644);
        		dup2(output_fd,1);
        		int return_code = execvp(cmd->argv [0], (char **)cmd->argv);
		        if (return_code < 0) {
		            perror("execvp failed");
		            exit(1);
		        }
		        close(output_fd);
        		break;
        	}
        	case 3:
        	{
        		int input_fd = open(cmd->input_file,O_RDONLY,0);
        		int output_fd = open(cmd->output_file, O_WRONLY | O_CREAT, 0644);
        		dup2(input_fd,0);
        		dup2(output_fd,1);
        		int return_code = execvp(cmd->argv [0], (char **)cmd->argv);
		        if (return_code < 0) {
		            perror("execvp failed");
		            exit(1);
		        }
        		close(input_fd);
        		close(output_fd);
        		break;
        	}
        	case 4:
        	{
        		int output_fd = open(cmd->output_file, O_RDWR | O_APPEND | O_CREAT, 0644);
        		dup2(output_fd,1);
        		int return_code = execvp(cmd->argv [0], (char **)cmd->argv);
		        if (return_code < 0) {
		            perror("execvp failed");
		            exit(1);
		        }
		        close(output_fd);
        		break;
        	}
        	case 5:
        	{
        		int input_fd = open(cmd->input_file,O_RDONLY,0);
        		int output_fd = open(cmd->output_file, O_WRONLY | O_CREAT, 0644);
        		dup2(input_fd,0);
        		dup2(output_fd,1);
        		int return_code = execvp(cmd->argv [0], (char **)cmd->argv);
		        if (return_code < 0) {
		            perror("execvp failed");
		            exit(1);
		        }
        		close(input_fd);
        		close(output_fd);
        		break;
        	}
        }

    } else if (pid < 0) {
        perror("fork failed");
        exit(1);
    }
    else
    {
    	int status;
    	waitpid(pid,&status,0);
    }
    return pid;
}
/* Helper function that forks pipes */
/*
This function will be called if there are pipes or 
redirection in the command given the by the user.
*/
int fork_pipes (int n, struct command_structure **cmd) {
    int i;
    int in, fd [2];
    in = 0;
    for (i = 0; i < n; ++i) {
        pipe (fd);
    	spawn_proc(in,fd[1],cmd[i]);
        close (fd [1]);
        in = fd [0];
    }
    if(in != 0)
    	dup2 (in, 0);
    spawn_proc(in,1,cmd[i]);
    int status;
    waitpid(-1,&status,0);
    return 0;
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
	char * kth_command = get_kth_command(up_count+1);
	parse_individual_command(kth_command);
	return 1;
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
int parse_individual_command(char* command)
{
	// The function will only work if up command is given
	if(parse_up_command(command))
		return 0;
	int pipe_count = give_count_of_pipes(command);
	if(pipe_count >= 1)
	{
		//separate on the basis of Pipe
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
		struct command_structure ** cmd;
		cmd = (struct command_structure **)malloc((pipe_count+1)*sizeof(struct command_structure *));
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
	  		sub_command = (struct command_structure *)malloc(sizeof(struct command_structure));
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
	  			else
	  			{
	  				command[index] = words_arr[j];
	  				index++;
	  			}
	  		}
	  		command[index] = NULL;
	  		sub_command->argv = command;
	  		cmd[i] = sub_command;
	  	}
	  	fork_pipes(pipe_count,cmd);
		return 0;
	}	
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
	if(is_builtin && builtin_index==12)
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
	else if(is_builtin && builtin_index==2)
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
				if(status == -1)
				{
					perror(strerror(errno));
					exit(0);
				}
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
		case 10:
			m_setenv(options_array,number_of_arguments);
			break;
		case 11:
			m_unsetenv(options_array,number_of_arguments);
			break;
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

