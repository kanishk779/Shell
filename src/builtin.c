#include "builtin.h"

extern int errno;

int p_info(char ** arguments,int number_of_args)
{
	char * path = (char *)malloc(40);
	if(number_of_args == 0)
	{
		// open a file
		pid_t pd = getpid();
		printf("pid - %u\n",pd);
		strcpy(path,"/proc/self/status");
	}
	else
	{
		printf("pid -%s\n",arguments[0]);
		sprintf(path,"/proc/%s/status",arguments[0]);
	}
	int fd = open(path,O_RDONLY);
	FILE * fp = fdopen(fd,"r");
	ssize_t read;
	char * buffer;
	size_t buf_size = 512;
	buffer = (char *)malloc(buf_size);
	while((read = getline(&buffer,&buf_size,fp)) != -1)
	{
		if(strncmp(buffer,"State",5) == 0)
		{
			printf("%s\n",buffer);
		}
		else if(strncmp(buffer,"VmSize",6) == 0)
		{
			printf("%s\n",buffer);
			break;
		}
	}
	close(fd);
	readlink("/proc/self/exe",buffer,buf_size);
	printf("%s\n",buffer);
	free(buffer);
	free(path);
	return 0;
}

void cronjob(char ** command, int t ,int p)
{
	int number_of_times = (p/t);
	for(int i=0;i<number_of_times;i++)
	{
		sleep(t);
		pid_t pid;
		if((pid = fork()) == 0)
		{
			signal (SIGINT, SIG_DFL);
	      	signal (SIGQUIT, SIG_DFL);
	      	signal (SIGTSTP, SIG_DFL);
	      	signal (SIGTTIN, SIG_DFL);
	      	signal (SIGTTOU, SIG_DFL);
	      	signal (SIGCHLD, SIG_DFL);
			execvp(command[0],command);
		}
		else if(pid < 0)
		{
			perror("error in forking the child\n");
			exit(1);
		}
		else
		{
			// parent 
			int status;
			pid_t pid_returned = waitpid(pid,&status,0);
		}
	}
}
int m_echo(char* user_msg)
{
	if(user_msg[0] == '$')
	{
		int len = strlen(user_msg);
		char * new_str = (char *)malloc(len);
		for(int i=1;i<len;i++)
			new_str[i-1] = user_msg[i];
		char * value = getenv(new_str);
		printf("%s\n",value);
		return 0;
	}
	printf("%s\n", user_msg);
	return 0;
}
void pwd(char * path)
{
	printf("%s\n", path);
}
void m_help(FILE * file){
  	if(file == NULL){
    	file = stdout;
  	}

  	fprintf(file,
         "[commands [OPTIONS] [Arguments];]\n"
         "  help           Print this help screen\n"
         "  pwd           Print the current working directory\n"
         "  pinfo           Print information related to process\n"
  		 "  history      shows the last few commands given\n"
  		 "  nightswatch      change the working directory\n"
         "  ls             list the information about the current_directory\n"
         "  quit    		exit the shell \n"
         "  echo           Helps in printing the message given to it\n");
}
void m_exit()
{
	printf("Goodbye !!\n");
	exit(0);
}