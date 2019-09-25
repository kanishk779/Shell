#include "environmentVariable.h"

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