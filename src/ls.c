#include "ls.h"
#include "absolutePath.h"
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
void printMonthDayYear(struct stat fileStats)
{
    char buf [100];
    struct tm * timeinfo;
    time_t timeStamp = fileStats.st_atime;
    timeinfo = localtime (&timeStamp);
    strftime (buf,100,"\t%h\t%e\t%Y\t",timeinfo);
    printf("\t%s",buf);
}
void printFileType(struct stat fileStats)
{
    printf( (S_ISDIR(fileStats.st_mode)) ? "d" : "");
    printf( (S_ISREG(fileStats.st_mode)) ? "-" : "");
    printf( (S_ISLNK(fileStats.st_mode)) ? "l" : "");

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
void printFileNames(char *name,struct stat *fileStats)
{
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