#include "headerDependencies.h"
#include "builtin.h"

extern int rear,front,size;
extern char **arr;
extern int used[QUEUE_SIZE];
extern char * home_directory;
extern int errno;
int count = 10;
static struct termios oldSettings;

void printMonthDayYear(struct stat fileStats)
{
    char buf [100];
    struct tm * timeinfo;
    time_t timeStamp = fileStats.st_atime;
    timeinfo = localtime (&timeStamp);
    strftime (buf,100,"\t%h\t%e\t%Y\t",timeinfo);
    printf("\t%s",buf);
}
void printPermissions(struct stat fileStats)
{
    mode_t mode = fileStats.st_mode;

    printf( (mode & S_IRUSR) ? "r" : "-");
    printf( (mode & S_IWUSR) ? "w" : "-");
    printf( (mode & S_IXUSR) ? "x" : "-");


    printf( (mode & S_IRGRP) ? "r" : "-");
    printf( (mode & S_IWGRP) ? "w" : "-");
    printf( (mode & S_IXGRP) ? "x" : "-");


    printf( (mode & S_IROTH) ? "r" : "-");
    printf( (mode & S_IWOTH) ? "w" : "-");
    printf( (mode & S_IXOTH) ? "x" : "-");
}
void printFileNames(char *name,struct stat *fileStats){
    char buff[100];
    if(S_ISREG(fileStats->st_mode))
    {
        if(fileStats->st_mode & S_IXOTH)
        {
            printf(ANSI_COLOR_GRN "\t%s", name);
            printf(ANSI_COLOR_RESET);
            return;
        }
        printf(ANSI_COLOR_RESET "\t%s", name);
    }
    if(S_ISDIR(fileStats->st_mode))
    {
        printf(ANSI_COLOR_BLU "\t%s", name);
        printf(ANSI_COLOR_RESET);
    }
    if(S_ISLNK(fileStats->st_mode)){
                printf(ANSI_COLOR_MAG "\t%s", name);
        printf(ANSI_COLOR_RESET);
    }
    readlink(name,buff,100);
    if(S_ISLNK(fileStats->st_mode)) // if the file is a symbolic link
            printf (" -> %s", buff);

}

void kbcleanup( void )
{
    tcsetattr( 0, TCSAFLUSH, &oldSettings );     /* restore old settings */
}

void kbsetup( void )
{
    tcgetattr( 0, &oldSettings );

    struct termios newSettings = oldSettings;

    newSettings.c_lflag &= ~ICANON;   /* disable line-at-a-time input */
    newSettings.c_lflag &= ~ECHO;     /* disable echo */
    newSettings.c_cc[VMIN]  = 0;      /* don't wait for characters */
    newSettings.c_cc[VTIME] = 0;      /* no minimum wait time */

    if ( tcsetattr( 0, TCSAFLUSH, &newSettings ) != 0 ){
        fprintf( stderr, "Unable to set terminal mode\n" );
        exit( 1 );    /* restore the terminal settings when the program exits */
    }
}

int getkey( void )
{
    char c;

    if ( read( STDIN_FILENO, &c, 1 ) == 0 )
        return '\0';
    else
        return c;
}
int numberOfCPU()
{
	int fd = open("/proc/interrupts",O_RDONLY);
	FILE * fp = fdopen(fd,"r");
	ssize_t read;
	char * buffer;
	size_t buf_size = 512;
	buffer = (char *)malloc(buf_size);
	int number_of_cpu=0;
	while((read = getline(&buffer,&buf_size,fp)) != -1)
	{
		char * ptr = strtok(buffer," ");
		if(strncmp(ptr,"CPU",3) == 0)
		{
			printf("%s  ",ptr);
			number_of_cpu++;
		}
	}
	printf("\n");
	close(fd);
	free(buffer);
	return number_of_cpu;
}
void print_num_of_interrupts(int seconds)
{
	int c;
    kbsetup();
    int fd = open("/proc/interrupts",O_RDONLY);
    FILE * fp = fdopen(fd,"r");
    ssize_t read;
    char * buffer;
    size_t buf_size = 512;
    buffer = (char *)malloc(buf_size);
    int num_of_cpu = numberOfCPU();
    for (;;)
    {
        sleep(seconds);
		int printed = 0;
		read = getline(&buffer,&buf_size,fp);
		while(printed < num_of_cpu)
		{
			for(int i=0;i<2;i++)
			{
				read = getline(&buffer,&buf_size,fp);
				if(i == 1)
				{
					char * interrupt_count = strtok(buffer," ");
					interrupt_count = strtok(NULL," ");
					//print the value of the number of interrupts
					printf("%s  ",interrupt_count);
				}
			}
			printed++;
			while((read = getline(&buffer,&buf_size,fp)) != -1)
			{
				char * ptr = strtok(buffer," ");
				if(strncmp(ptr,"CPU",3) == 0)
					break;
			}	
		}
		printf("\n");
	    if ( (c = getkey()) != '\0' )
	    {
	        if ( c ==(int) 'q' || c == (int)'Q' )
	            break;
	    }
	 	lseek(fd,0,SEEK_SET);
	}
	free(buffer);
	kbcleanup();
}
void print_dirty_mem(int seconds)
{
	int c;
    kbsetup();
    int fd = open("/proc/meminfo",O_RDONLY);
    FILE * fp = fdopen(fd,"r");
    ssize_t read;
    char * buffer;
    size_t buf_size = 512;
    buffer = (char *)malloc(buf_size);
    printf("dirty\n");
    for (;;)
    {
        sleep(seconds);
        for(int i=0;i<15;i++)
		read = getline(&buffer,&buf_size,fp);
		for(int i=0;i<5;i++)
		{
			read = getline(&buffer,&buf_size,fp);
			if(strncmp(buffer,"Dirty",5) == 0)
			{
				char *ptr = strtok(buffer," ");
				ptr = strtok(NULL," ");
				printf("%s kB\n", ptr);
				break;
			}
		}
	    if ( (c = getkey()) != '\0' )
	    {
	        if ( c ==(int) 'q' || c == (int)'Q' )
	            break;
	    }
	 	lseek(fd,0,SEEK_SET);
	}
	free(buffer);	
	kbcleanup();
}
int nightswatch(char ** arguments, int number_of_args)
{
	int seconds;
	char * command;
	if(strcmp(arguments[0],"-n") == 0)
	{
		seconds = atoi(arguments[1]);
		command = arguments[2];
	}
	else
	{
		seconds = 2;
		command = arguments[0];
	}
	if(strcmp(command,"interrupt") == 0)
	{
		print_num_of_interrupts(seconds);
	}
	else if(strcmp(command,"dirty") == 0)
	{
		print_dirty_mem(seconds);
	}
	else
	{
		printf("Only interrupt and dirty are supported\n");
	}
	return 0;
}
void printFileType(struct stat fileStats)
{
    printf( (S_ISDIR(fileStats.st_mode)) ? "d" : "");
    printf( (S_ISREG(fileStats.st_mode)) ? "-" : "");
    printf( (S_ISLNK(fileStats.st_mode)) ? "l" : "");

}
char* giveAbsolutePath(char *path)
{
    char *build_path  = (char *)malloc(255); 
    //char buf[255];
    if(path[0]=='/')
    {
        return path;
    }
   	getcwd(build_path,255);
    strcat(build_path,"/");
    strcat(build_path,path);
    return build_path;

}
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
void print_info(DIR * mydir,char * given_directory,struct dirent *myfile,int is_l_mentioned,int is_a_mentioned)
{
	mydir = opendir(given_directory);
	struct stat mystat;
	char buffer[1024];
	while((myfile = readdir(mydir)) != NULL)
	{
		if(myfile->d_name[0]=='.' && !is_a_mentioned)
			continue;
		if(is_l_mentioned)
		{
			// This is the standard order 
			sprintf(buffer, "%s/%s", given_directory, myfile->d_name);
        	lstat(buffer, &mystat);
        	printFileType(mystat);
            printPermissions(mystat);
   	        printf("\t%ld", mystat.st_nlink);
            printf("\t%s", getpwuid(mystat.st_uid)->pw_name);
            printf("\t%s", getgrgid(mystat.st_gid)->gr_name);
       	    printf("\t%ld ", mystat.st_size);
            printMonthDayYear(mystat);
            printFileNames(myfile->d_name, &mystat);
            printf("\n");
		}
		else
		{
			printf("%s\n",myfile->d_name);
		}
	}
	closedir(mydir);
}
int m_ls(char* current_path,char ** options,int number_of_args)
{
	DIR *mydir;
	struct dirent *myfile;
	char buffer[1024];
	int is_l_mentioned=false;
	int is_a_mentioned=false;
	int is_directory_given=false;
	char * given_directory = (char *)malloc(PATH_M);
	for(int i=0;i<number_of_args;i++)
	{
		if(strcmp(options[i],"-a") == 0)
			is_a_mentioned = true;
		else if(strcmp(options[i],"-l") == 0)
			is_l_mentioned = true;
		else if(strcmp(options[i],"-al")==0 || strcmp(options[i],"-la")==0)
		{
			is_l_mentioned = true;
			is_a_mentioned = true;
		}
		else
		{
			strcpy(given_directory,options[i]);
			is_directory_given = true;
		}
	}
	// what happens if someone gives a directory name but it is actually not
	if(is_directory_given)
	{
		char *some_directory = giveAbsolutePath(given_directory);
		// here check if some_directory is actually a directory or if it is even present
		DIR * m_dir = opendir(some_directory);
		if(m_dir){
			closedir(m_dir);
			print_info(mydir,some_directory,myfile,is_l_mentioned,is_a_mentioned);
		}
		else if(errno == ENOENT)
		{
			printf("directory does not exist\n");
		}
		return 0;
	}
	mydir = opendir(current_path);
	print_info(mydir,current_path,myfile,is_l_mentioned,is_a_mentioned);
	closedir(mydir);
	free(given_directory);
	return 0;
}
int print_error_for_cd()
{
	int printed = false;
	if(errno == ENOENT)
	{
		printed = true;
		printf("The named directory does not exist\n");
	}
	else if(errno == ENOTDIR)
	{
		printed = true;
		printf("A component of the path prefix is not a directory\n");
	}
	else if(errno == EACCES)
	{
		printed = true;
		printf("Search permission is denied for any component of the path name\n");
	}
	return printed;
}
int m_cd(char* current_path,char ** arguments,int number_of_args)
{
	if(number_of_args == 0)
	{
		printf("please mention the directory\n");
		return -1;
	}
	if(number_of_args >= 1)
	{
		if(number_of_args>1)
			printf("too many arguments are passed will consider only first one\n");
		if(strcmp(arguments[0],"~")==0)
		{
			int status = chdir(home_directory);
			if(status != 0)
			{
				int error = print_error_for_cd();
				if(error)
					return -1;
				else
					return 0;
			}
			else
				return 0;
		}
		arguments[0] = giveAbsolutePath(arguments[0]);
		int status = chdir(arguments[0]);
		if(status != 0)
		{
			int error = print_error_for_cd();
			if(error)
				return -1;
			else
				return 0;
		}
		else
			return 0;
	}
	return 0;
}
/*
This queue is implemented to maintain the history of the commands
*/
    
void display_Queue(int how_many)
{
   	if (front == -1) 
   	{ 
       	printf("\nYou have not given any commands till now\n"); 
       	return; 
   	} 
   	printf("History : \n"); 
   	if (rear >= front) 
   	{
   		int total = rear - front + 1;
   		if(how_many >= total)
   		{
   			// show everything
   			for(int i=rear;i>=front;i--)
   			printf("%s",arr[i]); 
   		}
   		else
   		{
   			for (int i = rear; i >= rear-how_many+1; i--) 
           	printf("%s",arr[i]);
   		}
   	} 
   	else
   	{
  		if(how_many >= (rear+1))
  		{
  			for (int i = rear; i>=0; i--) 
  			{
           		printf("%s", arr[i]);
       			int remaining = how_many-rear-1;
  		   		if(remaining >= (QUEUE_SIZE - front))
        		{
        		// show everything that is present in the queue
        			for(int i=QUEUE_SIZE-1;i>=front;i--)
        			printf("%s",arr[i]);
        		}
        		else
        		{
        			for(int i=QUEUE_SIZE-1;i>=QUEUE_SIZE-remaining;i--)
        			printf("%s",arr[i]);
        		}
        	}
  		}
  		else
  		{
  			for(int i= rear;i>=rear-how_many+1;i--)
  				printf("%s",arr[i]);
  		}		
    } 
}

int m_setenv(char ** arguments,int number_of_args)
{
	if(number_of_args == 0)
	{
		printf("The variable name and value needs to be given\n");
		return 0;
	}
	else if(number_of_args >= 3)
	{
		printf("Too many arguments only variable name and value needs to be given\n");
		return 0;
	}
	else if(number_of_args == 2)
	{
		setenv(arguments[0],arguments[1],1);
	}
	return 0;
}

int m_unsetenv(char ** arguments,int number_of_args)
{
	if(number_of_args == 0)
	{
		printf("You must tell which variable to unset\n");
		return 0;
	}
	else if(number_of_args > 1)
	{
		printf("You must only give one argument\n");
		return 0;
	}
	else
	{
		unsetenv(arguments[0]);
	}
	return 0;
}

void history(int how_many)
{
	count = how_many;
	display_Queue(count);
	count = 10;
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
         "  exit 		exit the shell \n"
         "  echo           Helps in printing the message given to it\n");
}
void m_exit()
{
	printf("Goodbye !!\n");
	exit(0);
}