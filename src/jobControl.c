#include "jobControl.h"
#include "headerDependencies.h"
#include "utils.h"
#include "main.h"
void show_jobs()
{
	int count = 1;
	job * temp = first_job;
	while(temp != NULL)
	{
		int stopped = job_is_stopped(temp);
		if(stopped)
			printf("[%d] Stopped %s [%u]\n",count,temp->job_name,temp->pgid);
		else
			printf("[%d] Running %s [%u]\n",count,temp->job_name,temp->pgid);
		count++;
		temp = temp->next;
	}
}
void remove_completed_jobs()
{
	job* prev = NULL;
	job* curr = first_job;
	while(curr)
	{
		int complete = job_is_completed(curr);
		if(complete)
		{
			if(prev != NULL)
			{
				prev->next = curr->next;
				curr = curr->next;
				free(curr);
			}
			else
			{
				if(curr->next == NULL)
				{
					first_job = NULL;
					curr = NULL;
				}
				else
				{
					first_job = curr->next;
					curr = curr->next;
					free(curr);
				}
			}
			continue;
		}
		prev = curr;
		curr = curr->next;
	}
}
void kjob(char ** arguments,int number_of_arguments)
{
	if(number_of_arguments < 2)
	{
		printf("Less arguments given\n");
		return;
	}
	int job_number = stringToInt(arguments[0]);
	int signal_number = stringToInt(arguments[1]);
	int count = 1;
	job * temp = first_job;
	int found_job = false;
	while(temp != NULL)
	{
		if(count == job_number)
		{
			found_job = true;
			signal(signal_number,SIG_DFL);
			break;
		}
		else
		{
			temp = temp->next;
		}
	}
	if(!found_job)
		printf("No job exist with this job number\n");
}
void fg(char ** arguments,int number_of_arguments)
{
	if(number_of_arguments > 1)
	{
		printf("too many arguments\n");
		return;
	}
	if(number_of_arguments == 0)
	{
		printf("give the job number\n");
		return;
	}
	// only count the background jobs while traversing
	int job_number = stringToInt(arguments[0]);
	remove_completed_jobs();
	//do_job_notification();
	int count = 0;
	job * temp = first_job;
	int found_job = false;
	while(temp != NULL)
	{
		if(!temp->foreground)
		{
			count++;
			if(count == job_number)
			{
				//put_job_in_foreground(temp,1);
				continue_job(temp,0);
				break;
			}
		}
		temp = temp->next;
	}
	if(!found_job)
		printf("job was not found\n");
}
// change from a stopped background job to running background job
void bg(char ** arguments,int number_of_arguments)
{
	if(number_of_arguments > 1)
	{
		printf("too many arguments\n");
		return;
	}
	if(number_of_arguments == 0)
	{
		printf("give the job number\n");
		return;
	}
	remove_completed_jobs();
	int job_number = stringToInt(arguments[0]);
	//do_job_notification();
	int count = 0;
	job * temp = first_job;
	int found_job = false;
	while(temp != NULL)
	{
		if(!temp->foreground)
		{
			count++;
			if(count == job_number)
			{
				put_job_in_background(temp,1);
				break;
			}
		}
		temp = temp->next;
	}
	if(!found_job)
		printf("job was not found\n");
}

// kill all the background processes
void overkill()
{
	job * temp = first_job;
	while(temp)
	{
		if(!temp->foreground)
		{
			if(kill(temp->pgid,SIGKILL) == -1)
				perror("error in killing job\n");
		}
		temp = temp->next;
	}
}