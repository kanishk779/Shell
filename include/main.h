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
This function displays the initial information to the user,
when starting the shell. It also initialises the variables.
*/
void init_shell();

/*
gives the relative path in-case the current directory is inside
the the home-directory(where this project resides).
gives absolute path in-case the current directory is the ancestor
of the home-directory.
@param[in]		curr_dir	current directory in shell
@param[in]		home_dir 	home directory determined in the
							beginning of the program.
@return 		the absolute or relative path.
*/ 
char * give_relative_or_absolute_path(char * curr_dir,char* home_dir);
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
*/
char ** fill_argument_array(int words);

/*
Shows the <username@system_name:curr_dir> prompt to the user 
after executing every command.
*/
void show_user_sys_name();

/*
Calls the appropriate builtin function passing it the options and arguments
@param[in]		builtin_index 	index of the builtin command to run
@param[in]		options_array	contains the options and arguments for the commands
@param[in]		number_of_args	number of args present in the options array
*/
void call_appropriate_function(int builtin_index,char ** options_array,int number_of_args);

/*
Free all the heap allocated memory so that no memory leaks take 
place once the program is over.
*/
void m_free();

/*
This function displays the various builtin command that this
shell supports.
@param[in]		file 		the file pointer to which the information
							is to be displayed.
*/
void m_help(FILE * file);

/*
reads a line of command from the terminal
*/
char * m_shell_read_line();

/*
initialises the empty job 
@return 		The empty job
*/
job* init_job();



/*
initialises an empty command_structure
*/
command_structure * init_command_structure();

/*
finds the job with mentioned pgid
*/
job *find_job (pid_t pgid);



int job_is_stopped (job *j);
int job_is_completed (job *j);
void put_job_in_foreground(job * j,int cont);
void wait_for_job (job *j);
void format_job_info (job *j, const char *status);
int mark_process_status (pid_t pid, int status);
void update_status (void);
int run_command(char * command);
void do_job_notification (void);
void mark_job_as_running (job *j);
void continue_job (job *j, int foreground);
void put_job_in_background(job * j,int cont);

#endif

	