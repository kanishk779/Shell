#ifndef _HISTORY_
#define _HISTORY_
#define QUEUE_SIZE 20
int rear,front,size;
char **arr;
int used[QUEUE_SIZE];
/*
Shows the history of the commands given to the shell
@param[in]		how_many		number of commands to show
*/
void history(int how_many);

void fill_Queue(char * str);
char* remove_from_Queue();
void display_Queue(int how_many);
char * get_kth_command(int k);
#endif

