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
#ifndef A3
#define A3
/* gets the system information*/
void get_system_info();
/*gets the usage of memory in kilobytes*/
void total_usage ();
/*gets core numbers*/
void core_numbers();
/*gets user information and writes it to the pipe*/
void get_user_info(int pipe_num);
/*helper to convert to GB*/
float convert_to_GB(unsigned long byte);
/*creates the string with memory graphics*/
char*print_graphics_mem(float vir_used, float vir_tot, float phy_used, float phy_tot, float prev);
void get_previous(float prev_vir[][1024], int i);
/*gets memory information  and writes it to the pipe*/
void usage_of_memory(int pipe_memory,char memory[][1024], float the_values[][1024],int iter, bool graphics, bool sequential, int samples,float prev);
/*gets cpu information  and writes it to the pipe*/
void cpu_info(int pipe,double cpu_arr[][1024], int i,bool graphics, int tdelay, bool sequential);

#endif