#ifndef _EXECUTE_
#define _EXECUTE_
#include "main.h"
int spawn_proc (int in, int out, job * j,struct command_structure *cmd);
int fork_pipes (int n,job * new_job);
#endif