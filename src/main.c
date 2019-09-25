#include "main.h"
#include "history.h"
void handler()
{
	int status;
	//pid_t w = waitpid(-1,&status,WNOHANG);
	// here we need to the job notification
	do_job_notification();
	// if(w == -1)
	// {
	// 	perror("There was error in wait_pid in handler\n");
	// 	exit(0);
	// }
	// if(WEXITSTATUS(status))
	// 	printf("The process with pid - %u exited \n",w);
	// else if(WIFSIGNALED(status))
	// 	printf("The process with pid - %u was signalled \n",w);
}
void init_shell()
{
	clear();
	shell_terminal = STDIN_FILENO;

	// while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp ()))
 //        kill (shell_pgid, SIGTTIN);

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
	builtins[0] = "ls"; // working(with error handling)
	builtins[1] = "cd"; // working(with error handling)
	builtins[2] = "echo"; // working
	builtins[3] = "quit"; // working
	builtins[4] = "history"; //working
	builtins[5] = "help"; // working
	builtins[6] = "clear";	// working
	builtins[7] = "pinfo";  //working
	builtins[8] = "nightswatch"; // working
	builtins[9] = "pwd"; // working
	builtins[10]= "setenv"; // working
	builtins[11]= "unsetenv"; // working
	builtins[12]= "cronjob"; // working
	builtins[13]= "overkill"; // does nothing
	builtins[14]= "kjob";
	builtins[15]= "bg";  // not working
	builtins[16]= "fg";  // not working
	builtins[17]= "jobs"; // working correctly

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
    	if (kill (-j->pgid, SIGCONT) < 0)
      		perror ("kill (SIGCONT)");
}
/* Check for processes that have status information available,
   blocking until all processes in the given job have reported.  */

void wait_for_job (job *j)
{
  int status;
  pid_t pid;

  do
    pid = waitpid (WAIT_ANY, &status, WUNTRACED);
  while (!mark_process_status (pid, status)
         && !job_is_stopped (j)
         && !job_is_completed (j));
}

/* Format information about job status for the user to look at.  */

void format_job_info (job *j, const char *status)
{
  fprintf (stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
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



/* Helper function that forks pipes */
/*
This function will be called if there are pipes or 
redirection in the command given the by the user.
*/
int mark_process_status (pid_t pid, int status)
{
  	job *j;
  	command_structure *p;

  	if (pid > 0)
    {
      /* Update the record for the process.  */
      	for (j = first_job; j; j = j->next)
        	for (p = j->first_command; p; p = p->next)
          	if (p->pid == pid)
            {
              	p->status = status;
              	if (WIFSTOPPED (status))
                	p->stopped = 1;
              	else
                {
                  	p->completed = 1;
                  	if (WIFSIGNALED (status))
                    	fprintf (stderr, "%d: Terminated by signal %d.\n",(int) pid, WTERMSIG (p->status));
                }
              return 0;
            }
      	fprintf (stderr, "No child process %d.\n", pid);
      	return -1;
    }
  	else if (pid == 0 || errno == ECHILD)
    /* No processes ready to report.  */
    return -1;
  	else {
    /* Other weird errors.  */
    perror ("waitpid");
    return -1;
  	}
}

/* Check for processes that have status information available,
   without blocking.  */

void update_status (void)
{
  	int status;
  	pid_t pid;

  	do
    	pid = waitpid (WAIT_ANY, &status, WUNTRACED|WNOHANG);
  	while (!mark_process_status (pid, status));
}



/* Notify the user about stopped or terminated jobs.
   Delete terminated jobs from the active job list.  */

void do_job_notification (void)
{
  job *j, *jlast, *jnext;
  command_structure *p;

  /* Update status information for child processes.  */
  update_status ();

  jlast = NULL;
  for (j = first_job; j; j = jnext)
    {
      jnext = j->next;

      /* If all processes have completed, tell the user the job has
         completed and delete it from the list of active jobs.  */
      if (job_is_completed (j)) {
        format_job_info (j, "completed");
        if (jlast)
          jlast->next = jnext;
        else
          first_job = jnext;
      // how to implement this function
        //free_job (j);
      }

      /* Notify the user about stopped jobs,
         marking them so that we won’t do this more than once.  */
      else if (job_is_stopped (j) && !j->notified) {
        format_job_info (j, "stopped");
        j->notified = 1;
        jlast = j;
      }

      /* Don’t say anything about jobs that are still running.  */
      else
        jlast = j;
    }
}

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
