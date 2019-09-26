#include "main.h"
#include "history.h"
#include "jobControl.h"
#include "callBuiltin.h"
#include "utils.h"
#include "parser.h"

// this the handler for the SIGHCHLD signal (raised when process is either completed or stopped)
void handler()
{
	int pid;
	int status;
	do{
		pid = waitpid(-1, &status, WUNTRACED | WNOHANG);
		update_job_table(pid, status);
	}while(pid > 0);
	
}
// updates the jobs linked list by updating the status of each job
int update_job_table(int pid, int status) 
{
	job *j;
  	command_structure *p;
  	if(pid > 0)
    {
      	for (j = first_job; j; j = j->next)
        	for (p = j->first_command; p; p = p->next)
          	{
          		if (p->pid == pid)
	            {
	              	p->status = status;
	              	if (WIFSTOPPED (status))
	                {
	                	j->foreground = false;
	                	p->stopped = 1;
	                	if(!j->foreground)
	                		printf("+ %d suspended \n", pid);
	                }
	              	else
	                {
	                  	p->completed = 1;
	                  	if(!j->foreground)
	                  		printf("%d completed \n", pid);
	                  	remove_completed_jobs();
	                  	if (WIFSIGNALED (status))
	                    	fprintf (stderr, "%d: Terminated by signal %d.\n",(int) pid, WTERMSIG (p->status));
	                }
	              	return 0;
	            }
          	}
      	fprintf (stderr, "No child process %d.\n", pid);
    }
  	else if (pid == 0 || errno == ECHILD)
    	perror("There is no such process which stopped or completed");
  	else 
	    perror ("waitpid");
  	return -1;
}
// call this function after execvp of a foreground process(it runs till child gives SIGCHLD)
void wait_for_job (job *j)
{
  int status;
  pid_t pid;
  while (!job_is_stopped (j));
}
// used for initialising the shell
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
	// list of builtins
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
	builtins[13]= "overkill"; 
	builtins[14]= "kjob";
	builtins[15]= "bg";  
	builtins[16]= "fg";  
	builtins[17]= "jobs"; 

	printf("------------------------This is shell inside shell ----------------\n");
}
int main()
{
	// initialise shell and its variables
	init_shell();
	signal(SIGCHLD,handler);
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
// show the prompt for the shell
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
// initialises the job and returns it
job * init_job()
{
	job * new_job = (job *)malloc(sizeof(job));
	new_job->next = NULL;
	new_job->command = NULL;
	new_job->first_command = NULL;
	new_job->pgid = -1;
	new_job->notified = 0;
	new_job->job_name = NULL;
	return new_job;
}
// initialises the command_structure and returns it
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
void put_job_in_foreground(job * j,int cont)	
{
	/* Put the job into the foreground.  */
  	tcsetpgrp (shell_terminal, j->pgid);

  	/* Send the job a continue signal, if necessary.  */
  	if (cont)
    {
      	tcsetattr (shell_terminal, TCSADRAIN, &j->tmodes);
      	if(kill (- j->pgid, SIGCONT) < 0)
        	perror ("kill (SIGCONT)");
    }

  	/* Wait for it to report.  */
  	wait_for_job (j);

  	/* Put the shell back in the foreground.  */
  	tcsetpgrp (shell_terminal, shell_pgid);

  	/* Restore the shell’s terminal modes.  */
  	tcgetattr (shell_terminal, &j->tmodes);
  	tcsetattr (shell_terminal, TCSADRAIN, &shell_tmodes);
}
void put_job_in_background(job * j,int cont)
{
	/* Send the job a continue signal, if necessary.  */
  	if (cont)
    	if (kill (- j->pgid, SIGCONT) < 0)
      		perror ("kill (SIGCONT)");
}

/* Format information about job status for the user to look at.  */
void format_job_info (job *j, const char *status)
{
  	fprintf (stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
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
// marks the job j as running again
void mark_job_as_running (job *j)
{
	command_structure *p;

	for (p = j->first_command; p; p = p->next)
	    p->stopped = 0;
	j->notified = 0;
}

/* Continue the job J.  */
void continue_job (job *j, int foreground)
{
  mark_job_as_running (j);
  if (foreground)
    put_job_in_foreground (j, 1);
  else
    put_job_in_background (j, 1);
}
