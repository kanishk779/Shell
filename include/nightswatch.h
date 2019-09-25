#ifndef _NIGHTSWATCH_
#define _NIGHTSWATCH_
#include "headerDependencies.h"

void kbcleanup( void );
void kbsetup( void );
int getkey( void );
int numberOfCPU();
void print_num_of_interrupts(int seconds);
void print_dirty_mem(int seconds);
int nightswatch(char ** arguments, int number_of_args);

#endif