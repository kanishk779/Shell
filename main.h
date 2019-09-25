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
#define QUEUE_SIZE 20
#define PATH_MAXIMUM 1024

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
gives the number of the words in the string given
@param[in]		str 		string in which the number of words are to returned
@return 		the number of words str
*/
int count_of_words_in_str(char * str);

/*
Gives the number of commands separated by semicolon
@param[in]		str 		string for which no. of commands are to be returned
@return 		the number of commands
*/
int number_of_commands_separated_by_semicolon(char *str);

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
gives the length of the string (in case strlen doesn't work)
@param[in]		str 		the string whose length is calculated
@return 		returns the length of the string
*/
int m_strlen(char *str);

/*
reads a line of command from the terminal
*/
char * m_shell_read_line();

/*
inserts the command int the job's linked list of commands
@param[in]		process 	the command to be inserted
@param[in]		j 			the job in which the command is to be inserted
*/
void insert_command_structure(command_structure * process,job *j);

/*
adds the job to the linked list of jobs
@param[in]		new_job		the job to be inserted into the global list of jobs
*/
void insert_job(job * new_job);

/*
initialises the empty job 
@return 		The empty job
*/
job* init_job();

/*
moves the background process into the foreground with specified job number
@param[in]		arguments 		job number of the background process
*/
void fg(char ** arguments ,int number_of_args);

/*
resumes the background process with specified job number
@param[in]		arguments 		job number of the background process
*/
void bg(char ** arguments, int number_of_args);

/*
removes the completed jobs from the linked list of the jobs
*/
void remove_completed_jobs();

/*
kills all the background processes
*/
void overkill();

/*
shows all the jobs which are running or stopped
*/
void show_jobs();

/*
sends a signal to the job
@param[in]		job_number 		to which the signal has to be sent
@param[in]		signal_number	which signal to send
*/
void kjob(char ** arguments,int number_of_args);

/*
initialises an empty command_structure
*/
command_structure * init_command_structure();

/*
finds the job with mentioned pgid
*/
job *find_job (pid_t pgid);

/*
converts a string to int
*/
int stringToInt(char * str);


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
int is_it_background(char * command);
#endif

	