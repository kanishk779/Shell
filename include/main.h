#ifndef _MAIN_H_
#define _MAIN_H_
#include "headerDependencies.h"
#define clear() printf("\033[H\033[J")
#define MAIN_TOK_DELIM " \t\r\n\a"
#define COMMAND_DELIM ";"
#define PIPE_DELIM "|"
#define REDIRECTION_DELIM " ><>>"
#define NO_OF_BUILTIN 18
#define HOST_NAME_MAXIMUM 100
#define LOGIN_NAME_MAXIMUM 100
#define MAX_COMMAND_LEN 500
#define PATH_MAXIMUM 1024
char* home_directory;
char hostname[HOST_NAME_MAXIMUM]; // also called as system/computer name
char *username;
char *builtins[NO_OF_BUILTIN];
pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
typedef struct command_structure
{
	struct command_structure *next; /* next command in pipeline */
    char **argv;					/* argument list for executing command */
    char * input_file;				/* input file (if redirection present)*/
    char * output_file;				/* output file (if redirection present) */
    int completed;					/* is the command completed */
    int stopped;					/* is the command stopped. */
    int status;						/* status of the command */
    pid_t pid;						/* pid of the command */
    int variant;					/* used for deciding type of redirection */
    char * command_name;			/* name of the command */
} command_structure;
typedef struct job
{
	struct job *next;           	/* next active job */
	char *command;              	/* command line, used for messages */
	command_structure *first_command;     /* list of processes in this job */
	pid_t pgid;                 	/* process group ID */
	int notified;              		/* true if user told about stopped job */
	struct termios tmodes;      	/* saved terminal modes */
	int foreground;					/* is the process foreground*/
	char * job_name;				/* name of the job */
} job;
job * first_job;

/*
Handles the SIGCHLD signal
*/
void handler();

/*
updates the job linked list
*/
int update_job_table(int pid, int status);

/*
waits for the foreground job to finish
*/
void wait_for_job (job *j);

/*
This function displays the initial information to the user,
when starting the shell. It also initialises the variables.
*/
void init_shell();

/*
Shows the <username@system_name:curr_dir> prompt to the user 
after executing every command.
*/
void show_user_sys_name();

/*
initialises the empty job 
@return 		The empty job
*/
job* init_job();

/*
initialises an empty command_structure
*/
command_structure * init_command_structure();

void put_job_in_foreground(job * j,int cont);

void put_job_in_background(job * j,int cont);

int job_is_stopped (job *j);

int job_is_completed (job *j);

void format_job_info (job *j, const char *status);

void mark_job_as_running (job *j);

void continue_job (job *j, int foreground);

#endif

	