#ifndef _NIGHTSWATCH_
#define _NIGHTSWATCH_
#include "headerDependencies.h"

void kbcleanup( void );
void kbsetup( void );
int getkey( void );
int numberOfCPU();
void print_num_of_interrupts(int seconds);
void print_dirty_mem(int seconds);
/*
A modified version of watch command (supports )
@param[in]		arguments		options for the command and type of command
@param[in]		number_of_args	number of char pointers in arguments
@return 		0 on success , -1 on failure
*/
int nightswatch(char ** arguments, int number_of_args);

#endif