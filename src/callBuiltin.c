#include "callBuiltin.h"
#include "headerDependencies.h"
#include "builtin.h"
#include "history.h"
#include "environmentVariable.h"
#include "ls.h"
#include "cd.h"
#include "nightswatch.h"
#include "utils.h"
#include "jobControl.h"
void call_appropriate_function(int builtin_index,char ** options_array,int number_of_arguments)
{
	char* curr_directory = (char *)malloc(PATH_MAXIMUM * sizeof(char));
	if(getcwd(curr_directory,PATH_MAXIMUM)==NULL)
	{
		perror("error in getting curr_directory when calling builtin\n");
		exit(1);
	}
	int status;
	switch(builtin_index)
	{
		case 0:
			status = m_ls(curr_directory,options_array,number_of_arguments);
			if(status == -1)
			{
				perror("ls function was not able to get executed\n");
			}
			break;
		case 1:
			status = m_cd(curr_directory,options_array,number_of_arguments);
			if(status == -1)
			{
				perror("cd function was not able to get executed\n");
			}
			break;
		case 3:
			m_exit();
			break;
		case 4:
			if(number_of_arguments == 0)
				history(10);
			else
			{
				int len = strlen(options_array[0]);
				int power = len - 1;
				int number_of_commands=0;
				for(int i=0;i<len;i++)
				{
					number_of_commands += (options_array[0][i]-'0')*pow(10,power);
					power--;
				}
				history(number_of_commands);
			}	
			break;
		case 5:
			m_help(NULL);
			break;
		case 6:
			clear();
			break;
		case 7:
			p_info(options_array,number_of_arguments);
			break;
		case 8:
			nightswatch(options_array,number_of_arguments);
			break;
		case 9:
			{
				char * current_directory;
				current_directory = (char *)malloc(PATH_MAXIMUM * sizeof(char));
				if(getcwd(current_directory,PATH_MAXIMUM) != NULL)
				{
					pwd(current_directory);
					free(current_directory);
				}
				else
					printf("Error in getting the curr_directory\n");
				break;
			}
		case 10:
			m_setenv(options_array,number_of_arguments);
			break;
		case 11:
			m_unsetenv(options_array,number_of_arguments);
			break;
		case 13:
			overkill();
			break;
		case 14:
			kjob(options_array,number_of_arguments);
			break;
		case 15:
			bg(options_array,number_of_arguments);
			break;
		case 16:
			fg(options_array,number_of_arguments);
			break;
		case 17:
			show_jobs();
			break;
		default:
			perror("Are you sane??\n");
	}
	free(curr_directory);
}