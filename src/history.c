#include"history.h"
#include"headerDependencies.h"
#include"main.h"
int count = 10;

char* remove_from_Queue()
{
    if (front == -1) 
    { 
        printf("\nNO commands have been given till now\n"); 
        return NULL; 
    } 
  
    char* data = arr[front]; 
    if (front == rear) 
    { 
       	front = -1; 
       	rear = -1; 
    } 
    else if (front == size-1) 
       	front = 0; 
    else
        front++; 
    return data; 
}
void history(int how_many)
{
	count = how_many;
	display_Queue(count);
	count = 10;
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
   			printf("%s\n",arr[i]); 
   		}
   		else
   		{
   			for (int i = rear; i >= rear-how_many+1; i--) 
           	printf("%s\n",arr[i]);
   		}
   	} 
   	else
   	{
  		if(how_many >= (rear+1))
  		{
  			for (int i = rear; i>=0; i--) 
  			{
           		printf("%s\n", arr[i]);
       			int remaining = how_many-rear-1;
  		   		if(remaining >= (QUEUE_SIZE - front))
        		{
        		// show everything that is present in the queue
        			for(int i=QUEUE_SIZE-1;i>=front;i--)
        			printf("%s\n",arr[i]);
        		}
        		else
        		{
        			for(int i=QUEUE_SIZE-1;i>=QUEUE_SIZE-remaining;i--)
        			printf("%s\n",arr[i]);
        		}
        	}
  		}
  		else
  		{
  			for(int i= rear;i>=rear-how_many+1;i--)
  				printf("%s\n",arr[i]);
  		}		
    } 
}
void fill_Queue(char * str)
{
    if(str[0] == '\033')
      return;
    if ((front == 0 && rear == size-1) || (rear == (front-1)%(size-1))) 
    { 
     	// we first need to deque and then enque
        remove_from_Queue();
        fill_Queue(str); 
        return; 
    }
    else if (front == -1)
    { 
       	front = rear = 0;
       	int len = strlen(str);
       	if(used[rear])
       	arr[rear] = (char *)realloc(arr[rear],len+1);
       	else
       	arr[rear] = (char *)malloc(len+1); 
       	strcpy(arr[rear],str);
       	used[rear] = 1;
   	} 
    else if (rear == size-1 && front != 0) 
    { 
       	rear = 0; 
       	int len = strlen(str);
       	if(used[rear])
       	arr[rear] = (char *)realloc(arr[rear],len+1);
       	else
       	arr[rear] = (char *)malloc(len+1);
       	strcpy(arr[rear],str);
       	used[rear] = 1; 
   	} 
    else
    {
       	rear++; 
       	int len = strlen(str);
       	if(used[rear])
       	arr[rear] = (char *)realloc(arr[rear],len+1);
       	else
       	arr[rear] = (char *)malloc(len+1);
       	strcpy(arr[rear],str);
       	used[rear] = 1; 
    }
}
char * get_kth_command(int k)
{
	int count = 1;
	int rear_temp = rear;
  if(k == 1 && rear>=0 && rear<QUEUE_SIZE)
    return arr[rear];
  else
  {
    printf("not enough commands in the history\n");
    return NULL;
  }
	while(rear_temp != front)
	{
		if(count == k && rear_temp>=0 && rear_temp<QUEUE_SIZE)
		{
			return arr[rear_temp];
		}
		else
		{
			rear_temp = (rear_temp - 1 + QUEUE_SIZE)%QUEUE_SIZE;
			count++;
		}
	}

	printf("User has not given that many commands till now\n");
	return NULL;
}