###########################################################################################################################################################################################
Assignment 3: System Monitoring Tool -- Concurrency & Signals  
###########################################################################################################################################################################################
Vaibhav Lakshmi Santhanam (1007642400) santha26
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Note to the user:
-----------------
A makefile has been created for assignment 3 called Makefile
The whole assignment has been split into two files 
A3main.c stats_functions.c and a header file A3.h which contains the header for the function inside stats_functions.c

Note -lm needs to be added and had been added in the makefile
"make: Nothing to be done for 'all'." In case this message is shown pls use make clean command and then call make again to recompile the flags.
the executable is named stats

so please call ./stats and the flags accordingly to run the program 

the flag --user and --sequential does not work together 
--user and --graphics dont work together 
--graphics --user --sequential do not work together 
Error checking has been done with fprintf and stderr
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
(Single flag calls)
-------------------
a)--system or --s: Should display the system information i.e memory usage,number of cores,cpu usage and system information

b)--user or --u:Should display the user information and the session they are connected to with the machine detail

c) --sequential or --se :Should display the system information along with users in a sequential manner

d) --samples=N : Should display 'N' information 

e) --tdelay=N : Should display information in'N' seconds

f) positional arguments 
   EG: 8 2 
   Should display 8 samples in 2 seconds
   Note: single number positional argument doessnt work  i.e 8 wont do 8  samples in 10 seconds. Instead use --samples=8 
h) --graphics or --g: Should display the information with the graphics feature 
   
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



The A3main.c file contains : 
---------------------------
This file mainly contains everything from forking to getting the information back from pipes and printing in the parent process

Signal Handler functions
-------------------------

1) void handle_sigint(int sig) - Handles the ctrl c signal
2) void handle_sigtstp(int sig)  - Handles the ctrl z signal

forking and pipes and reading
-----------------------------------
1)pid_t get_user(int user_pipe[2]) 
  /* forks to get the user information in each iteration in main loop and passes the writing end of the pipe*/



2)void read_users_from_child(int pipe_num,size_t bytes_read)
 /* read from the pipe and prints it in the main process*/



3)pid_t get_mem(int pipe_m[2],char memory[][1024],float the_values[][1024],int i, bool graphics,bool sequential,int samples,float prev)
/* forks to get the memory information in each iteration in main loop and passes the writing end of the pipe*/




4)void read_mem_from_child(int pipe_num,size_t bytes_read)
/* read from the pipe and prints it in the parent process*/





5)pid_t get_cpu(int pipe_cpu[2],double cpu_arr[][1024], int i,bool graphics, int tdelay,bool sequential)
  /* forks to get the cpu information in each iteration in main loop and passes the writing end of the pipe*/






6)void read_cpu_from_child(int pipe_num,size_t bytes_read)
 /* read from the pipe and prints it in the parent process*/

-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
stats_functions.c contains:
1)void get_system_info() -: gets the system information such as System Name,Machine Name,Version,Release,Architecture using 
                            the utsname structure within the <sys/utsname.h> library and displays the information as wanted.



2)void get_user_info(int pipenum) -: Displays the user information such as username,host and session by using the <utmp.h> library
                          setutent(),getutent(),endutent()is used. The info is written to the pipe 
                          





3)float convert_to_GB(unsigned long byte) -: Helper function to Convert bytes to Giga bytes makes used of <math.h> library 
                                               Parameter: unsigned long byte
                                               Return: float value of GB.



4)void total_usage ()-: Displays the The total usage of memory in Kilobytes.
                         



5)void core_numbers() -: Display the cores. Calculated using the /proc/stat/ file and counts the number of cpus excluding the first line of the file.


6)char* print_graphics_mem(float vir_used, float vir_tot, float phy_used, float phy_tot, float prev)
/* produces string with the graphics*/






7)void usage_of_memory(int pipe_memory,char memory[][1024], float the_values[][1024],int iter, bool graphics, bool sequential, int samples,float prev) -:report how much utilization of memory is being done (report used and free memory) makes use of the sysinfo struct in <sys/sysinfo.h>library to access the memory information.
  and  the information is written to the pipe
  
   the formula used for calculating the total physical , the total virtual , the used physical and used virtual are 
  
   Total physical memory: totalram
   Used physical memory: totalram - freeram
   Total virtual memory: total swap + freeswap
   Used virtual memory : (totalram - freeram) + (total swap - freeswap)
   
   and they are converted to GB by the helper function
 
8)void cpu_info(int pipe_cpu,double cpu_arr[][1024], int i,bool graphics, int tdelay,bool sequential){
	/* Reads the first line of  /proc/stat file at 2 different time stamps to find the cpu_usage uses fscanf to get the information and 
	store it in the various variables required for calculation and writes it to the pipe */

NOte: for graphics round function has been used so if the change is like 0.098 it would become 1


-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#######################################################################################################################################################################################

The libraries used 
==================
#include<stdio.h>
#include<stdlib.h>
#include<sys/resource.h>
#include<sys/utsname.h> 
#include<sys/sysinfo.h>
#include<utmp.h>
#include<math.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include <sys/wait.h>
#include <signal.h>
#########################################################################################################################################################################################

The resources used 
------------------
 https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-uname-display-current-operating-system-name
 https://man7.org/linux/man-pages/man5/utmp.5.html
 https://man7.org/linux/man-pages/man2/sysinfo.2.html
 https://www.ibm.com/docs/en/zos/2.2.0?topic=functions-getrusage-get-information-about-resource-utilization */
 https://www.linuxhowtos.org/System/procstat.html
https://www.delftstack.com/howto/c/pipe-in-c/
######################################################################################################################################################################################


