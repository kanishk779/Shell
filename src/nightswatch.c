#include "nightswatch.h"

static struct termios oldSettings;

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