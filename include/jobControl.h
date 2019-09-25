#ifndef _JOB_CONTROL_
#define _JOB_CONTROL_

void show_jobs();
void remove_completed_jobs();
void kjob(char ** arguments,int number_of_arguments);
void fg(char ** arguments,int number_of_arguments);
void bg(char ** arguments,int number_of_arguments);
void overkill();
#endif