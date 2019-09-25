#include "execute.h"
#include "headerDependencies.h"
int spawn_proc (int in, int out, job * j,struct command_structure *cmd) {
	// 5 cases can occur
	// case 0 -> no redirection is present
	// case 1 -> cmd < input
	// case 2 -> cmd arguments > somefile
	// case 3 -> cmd < input > somefile
	// case 4 -> cmd arguments >> somefile
	// case 5 -> cmd < input >> somefile
    pid_t pid;
    if ((pid = fork ()) == 0) {
    	if (j->pgid == -1) j->pgid = pid;
      	setpgid (pid, j->pgid);
        if (in != 0) {
            dup2 (in, 0);
            close (in);
        }
        if (out != 1) {
            dup2 (out, 1);
            close (out);
        }
        signal (SIGINT, SIG_DFL);
      	signal (SIGQUIT, SIG_DFL);
      	signal (SIGTSTP, SIG_DFL);
      	signal (SIGTTIN, SIG_DFL);
      	signal (SIGTTOU, SIG_DFL);
      	signal (SIGCHLD, SIG_DFL);
      

      	
      	// what is to be done here
      	if (j->foreground)
        	tcsetpgrp (shell_terminal, j->pgid);
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
    	cmd->pid = pid;
    	if (j->pgid == -1) j->pgid = pid;
      	setpgid (pid, j->pgid);
      	tcsetpgrp(shell_terminal,j->pgid);
    }
    return pid;
}
int fork_pipes (int n,job * new_job) {
    command_structure* i;
    int in, fd [2];
    in = 0;
    for (i = new_job->first_command; i; i=i->next) {
        pipe (fd);
        if(i->next)
    		spawn_proc(in,fd[1],new_job,i);
    	else
    		spawn_proc(in,1,new_job,i);
        close (fd [1]);
        in = fd [0];
    }
    format_job_info(new_job,"job started");
    if(new_job->foreground)
    	put_job_in_foreground(new_job,0);
    else
    	put_job_in_background(new_job,0);
    return 0;
}
