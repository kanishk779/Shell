# Assignment1_CSE_SHELL

This is a sample Shell which is an assignment that i did during my Operating Systems course in third semester.
The requirements of the assignment is as follows:-

1. The prompt should display in the following format **<username@system_name:curr_dir>** .![promp](https://github.com/kanishk779/Assignment1_CSE_SHELL/blob/master/Resources/prompt.png)   
2. The shell should support different builtin commands such as **ls**,**pwd**,**cd**,**echo**, etc. .![promp](https://github.com/kanishk779/Assignment1_CSE_SHELL/blob/master/Resources/pwdLs.png)  
3. The builtin commands should not use the execvp() or similar other such functions, i.e students need to implement them..![promp](https://github.com/kanishk779/Assignment1_CSE_SHELL/blob/master/Resources/echo.png)   
4. Particularly **ls** should also support options such as **-a**,**-l**,**-al/-la** or some_directory name.  
5. The system commands, both with and without the arguments should be supported.  
6. For the system commands, use of execvp() is allowed.  
7. The background processes must also be supported _for example_ **ls &**.  
8. **pinfo** command which prints the information related to the process whose pid is given as an argument to it..![promp](https://github.com/kanishk779/Assignment1_CSE_SHELL/blob/master/Resources/pinfo.png)   
9. In case there is no argument given to the pinfo command then it should give info about the shell program.  
10. Status of the background process that returns after completion must be printed.  
11. **nightswatch** which will support one option **-n seconds** and two different commands, intterupt and dirty.  .![promp](https://github.com/kanishk779/Assignment1_CSE_SHELL/blob/master/Resources/nightswatch.png) 
12. Press q or Q to exit the nightswatch command.
13. **interrupt** command will print the number of interrupts by keyboard controller after every specified number of **seconds**.  
14. **dirty** command will print the size of the part of memory which is dirty.  
15. Also the **history** commands need to be supported which shows the last few commands which the user entered. 
.![promp](https://github.com/kanishk779/Assignment1_CSE_SHELL/blob/master/Resources/History.png)   
16. The user can also specify the number of commands to show in history as an argument.
17. Multiple commands can be given at once separated by semi-colon. 
18. To exit the shell program type the command **exit**.

## How to run the Shell

To clone and run this application, you will need Git. From your command line:

``` bash
#clone this repository 
$ git clone https://github.com/kanishk779/Assignment1_CSE_SHELL

#go into the directory Assignment1_CSE_SHELL
$ cd Assignment1_CSE_SHELL

#build the project using meson build system
$ meson build
$ ninja -C build

#go into the directory build
cd build

#build run the shell 
$ ./output

#for doing the cleanup first ensure you are in Assignment1_CSE_SHELL directory
$ cd ..

#remove the directory build
$ rm -rf build
```

## NOTE

1. The **pinfo** will not work for mac Operating system because the /proc directory does not exist.
2. Similarly the **nightswatch** command will not work for the mac Operating system.



 
