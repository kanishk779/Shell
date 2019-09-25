#include "cd.h"
#include "absolutePath.h"
extern char * home_directory;
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