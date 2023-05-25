#include "A3.h"

//Vaibhav Lakshmi Santhanam


void get_system_info(){
	/*the library utsname.h holds the structure struct utsname we can use that struct to access
	the key system information  https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-uname-display-current-operating-system-name */

			    struct utsname info;
			    struct utsname *info_ptr=&info;
				int flag = uname(info_ptr); // will store -1 if error else stores 0
				if ( flag<0)
				{
				 fprintf(stderr, "%s", "Error in get_system_info() when accessing utsname struct.\n");
                 exit(1);
				}
				else
				{
						 printf("--------------------------------------------------------------\n");
						 printf (" ### System Information ### \n");
						 printf("System Name = %s\n", info_ptr->sysname);// to access the system name
						 printf("Machine Name = %s\n",info_ptr->nodename); // node name contains the machine name
						 printf("Version = %s\n",info_ptr->version);// to access the version
						 printf("Release = %s\n", info_ptr->release);// to access the release
						 printf("Architecture = %s\n",info_ptr->machine); // to access the architecture
						 printf("--------------------------------------------------------------\n");
				}
				

}

void total_usage (){

	/* gets the total usage in kilobytes using rusage https://www.ibm.com/docs/en/zos/2.2.0?topic=functions-getrusage-get-information-about-resource-utilization */
	struct rusage usage_info;
	struct rusage* ptr = &usage_info;
	int flag = getrusage(RUSAGE_SELF,ptr);
	if ( flag<0)
	{
		fprintf(stderr, "Error in total_usage(): getrusage returned a negative number\n");
        exit(1);
	
	}
	else 
	{
		printf (" Memory Usage : %ld Kilobytes\n",ptr->ru_maxrss);
		printf("--------------------------------------------------------------\n");
	}
}

void core_numbers(){

	/*gets the core numbers for the /proc/stat file   https://www.linuxhowtos.org/System/procstat.htm */
	FILE *file_ptr=fopen("/proc/stat","r");
	int count=0;
	char characters[30];
	//int num_cores=0;
   if( file_ptr!=NULL)
	{
		while (fgets(characters,sizeof(characters),file_ptr)!=NULL)
		{
			if (strncmp(characters,"cpu",3)==0)
			{
				count++;
				
			}
		}
	}
	else{
		fprintf(stderr, "Error: file pointer is NULL\n");
       exit(1);
	}
	printf("--------------------------------------------------------------\n");
	printf(" Number of cores :%d \n",count-1); // one is subtracted to exclude the first line
	
    fclose(file_ptr);


}
void get_user_info(int pipe_num){
	
	/* gets the user info such as username, host and session from utmp user_info https://man7.org/linux/man-pages/man5/utmp.5.html
     the functions getutent, setutent and endutent are used*/
	
	struct utmp* user_info;
	setutent();
	user_info = getutent(); // reads a line from the current file position in the utmp file 
	char buffer[1024];
    
	snprintf(buffer,sizeof(buffer),"--------------------------------------------------------------\n");
	write(pipe_num,buffer,strlen(buffer));
	snprintf(buffer,sizeof(buffer)," ### Sessions/users ### \n");
	write(pipe_num,buffer,strlen(buffer));
	 
	while ( user_info!=NULL)
	{
		if (user_info->ut_type==7)
		{
			snprintf(buffer,sizeof(buffer),"%s     %s (%s)\n",user_info->ut_user,user_info->ut_line,user_info->ut_host);
            write(pipe_num,buffer,strlen(buffer));
		}
	user_info=getutent();
    }
    endutent();
 //printf("--------------------------------------------------------------\n");
}

float convert_to_GB(unsigned long byte){

	/* Converts the bytes to giga bytes */

    float GB = byte/pow(1024,3);
	return GB;

}

void get_previous(float prev_vir[][1024], int i){
    /*gets the prev vir used value*/
	struct sysinfo memo_info;
    struct sysinfo *memo_info_ptr=&memo_info;
   int flag = sysinfo(memo_info_ptr);
if( flag<0){
	fprintf(stderr, "%s", "Error in usage_of_memory() when accessing sysinfo struct.\n");
    exit(1);
}		  

float vir_used = (memo_info_ptr->totalram-memo_info_ptr->freeram)+(memo_info_ptr->totalswap-memo_info_ptr->freeswap);
prev_vir[i][0] = convert_to_GB(vir_used);
}

char* print_graphics_mem(float vir_used, float vir_tot, float phy_used, float phy_tot, float prev) {
    /* returns a string for graphics for memory usage*/
    float change = vir_used - prev;
    float change_round = round(change * 100.0) / 100.0;

    char* buffer = malloc(1024); // dynamically allocates space for buffer
    buffer[0] = '\0';

    char temp[1000];
    sprintf(temp, "%.2f GB / %.2f GB  -- ", phy_used, phy_tot);
    strcat(buffer, temp);

    sprintf(temp, "%.2f GB / %.2f GB      |", vir_used, vir_tot);
    strcat(buffer, temp);

    if (change_round > 0.00) {
        for (int i = 0; i < change_round * 100; i++) {
            strcat(buffer, "#");
        }
        strcat(buffer, "*");
    } else if (change_round < 0.00) {
        for (int i = 0; i > change_round * 100; i--) {
            strcat(buffer, ":");
        }
        strcat(buffer, "@");
    } else if (change_round == 0.00) {
        if (signbit(change_round)) {
            strcat(buffer, "|@");
        } else {
            strcat(buffer, "|o");
        }
    }

    sprintf(temp, " %.2f (%.2f)\n", change_round, vir_used);
    strcat(buffer, temp);

    return buffer;
}


void usage_of_memory(int pipe_memory,char memory[][1024], float the_values[][1024],int iter, bool graphics, bool sequential, int samples,float prev){

char buffer[1024];
struct sysinfo memo_info;
int i=iter;
if(i==0&&!sequential){
snprintf(buffer,sizeof(buffer),"### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
write(pipe_memory,buffer,strlen(buffer));
}

i++;
struct sysinfo *memo_info_ptr=&memo_info;
int flag = sysinfo(memo_info_ptr);
if( flag<0){
	fprintf(stderr, "%s", "Error in usage_of_memory() when accessing sysinfo struct.\n");
    exit(1);
}		  

float phys_tot = memo_info_ptr->totalram;
float phys_used = phys_tot-(memo_info_ptr->freeram);
float vir_tot = phys_tot +(memo_info_ptr->totalswap);
float vir_used = (memo_info_ptr->totalram-memo_info_ptr->freeram)+(memo_info_ptr->totalswap-memo_info_ptr->freeswap);

sprintf(memory[iter]," %.2f GB /%.2f GB  --   %.2f GB /%.2f GB\n", convert_to_GB(phys_used),convert_to_GB(phys_tot),convert_to_GB(vir_used),convert_to_GB(vir_tot));

the_values[i][0]=convert_to_GB(vir_used);
if (sequential && !graphics){
	for (int j=0; j<i; j++){
		if(j==0){
		snprintf(buffer,sizeof(buffer),"### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
        write(pipe_memory,buffer,strlen(buffer));
		}
		if( j==iter){
			snprintf(buffer,sizeof(buffer)," %s",memory[j]);
	        write(pipe_memory,buffer,strlen(buffer));
		}
		else{
			snprintf(buffer,sizeof(buffer),"\n");
			write(pipe_memory,buffer,strlen(buffer));
		}
	}
	for(int prev_val=i;prev_val<samples;prev_val++){
        snprintf(buffer,sizeof(buffer),"\n");
		write(pipe_memory,buffer,strlen(buffer));
    }
}
else if(graphics && sequential){
	for (int j=0; j<i; j++){
	if(j==0){
	snprintf(buffer,sizeof(buffer),"### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
    write(pipe_memory,buffer,strlen(buffer));
    }
	if(iter==0){
	snprintf(buffer,sizeof(buffer),"%.2f GB / %.2f GB  -- %.2f GB / %.2f GB      |o  %.2f (%.2f)\n",convert_to_GB(phys_used),convert_to_GB(phys_tot),convert_to_GB(vir_used),convert_to_GB(vir_tot),convert_to_GB(vir_used),0.00);
	write(pipe_memory,buffer,strlen(buffer));
	}
	else if(j==iter){
	char* graphics_mem_string = print_graphics_mem(convert_to_GB(vir_used), convert_to_GB(vir_tot), convert_to_GB(phys_used), convert_to_GB(phys_tot), prev);
    strcpy(buffer, graphics_mem_string);
    write(pipe_memory, buffer, strlen(buffer));
    free(graphics_mem_string);
	}
	else{
		snprintf(buffer,sizeof(buffer),"\n");
		write(pipe_memory,buffer,strlen(buffer));
	}
}
for(int prev_val=i;prev_val<samples;prev_val++){
     snprintf(buffer,sizeof(buffer),"\n");
	 write(pipe_memory,buffer,strlen(buffer));
}
}
else if(graphics && !sequential){
	if( iter == 0){
	snprintf(buffer, sizeof(buffer),"\033[H");
	write(pipe_memory,buffer,strlen(buffer));
	snprintf(buffer, sizeof(buffer),"\033[%dB",iter+4);
    write(pipe_memory,buffer,strlen(buffer));
	snprintf(buffer,sizeof(buffer),"%.2f GB / %.2f GB  -- %.2f GB / %.2f GB      |o  %.2f (%.2f)\n",convert_to_GB(phys_used),convert_to_GB(phys_tot),convert_to_GB(vir_used),convert_to_GB(vir_tot),convert_to_GB(vir_used),0.00);
	write(pipe_memory,buffer,strlen(buffer));
}
else{
		char* graphics_mem_string = print_graphics_mem(convert_to_GB(vir_used), convert_to_GB(vir_tot), convert_to_GB(phys_used), convert_to_GB(phys_tot), prev);
		snprintf(buffer, sizeof(buffer),"\033[H");
		write(pipe_memory,buffer,strlen(buffer));
		snprintf(buffer, sizeof(buffer),"\033[%dB",iter+4);
	    write(pipe_memory,buffer,strlen(buffer));
        strcpy(buffer, graphics_mem_string);
        write(pipe_memory, buffer, strlen(buffer));
        free(graphics_mem_string);
}
}
else{
	snprintf(buffer, sizeof(buffer),"\033[H");
	write(pipe_memory,buffer,strlen(buffer));
	snprintf(buffer,sizeof(buffer),"\033[%dB %s",iter+4,memory[iter]);
	write(pipe_memory,buffer,strlen(buffer));
}
}
 
void cpu_info(int pipe_cpu,double cpu_arr[][1024], int i,bool graphics, int tdelay,bool sequential){
	/* Reads the first line of  /proc/stat file at 2 different time stamps to find the cpu_usage uses fscanf to get the information and 
	store it in the various variables required for calculation */

    float user1, nice1, system1, idle1, iowait1, irq1, softirq1;
    float user2, nice2, system2, idle2, iowait2, irq2, softirq2;
    float total1;
    float total2;
    float cpu_usage1;
	char buffer[1024];
    FILE *fp = fopen("/proc/stat", "r");    // reads the /proc/stat file
    if(fp == NULL){
        fprintf(stderr, "%s", "The file could not be opened. Exiting.\n"); 
        exit(1);
    }
    fscanf(fp,"cpu %f %f %f %f %f %f %f",&user1, &nice1, &system1, &idle1, &iowait1, &irq1, &softirq1);
    total1 = user1 + nice1 + system1 + iowait1 + irq1 + softirq1+idle1;
	float totall=total1-idle1;
    fclose(fp);
    sleep(tdelay);
    FILE *fp1 = fopen("/proc/stat", "r");
	
    if(fp1 == NULL){
        fprintf(stderr, "%s", "The file could not be opened. Exiting.\n"); 
        exit(1);
    }
    fscanf(fp1,"cpu %f %f %f %f %f %f %f",&user2, &nice2, &system2, &idle2, &iowait2, &irq2, &softirq2);
    total2 = user2 + nice2 + system2 + iowait2 + irq2 + softirq2+idle2;
	float totall2=total2-idle2;
    fclose(fp1);
    cpu_usage1 = fabs((double)(((totall2-totall))/(total2-total1))*100);
    cpu_arr[i][0]=cpu_usage1;
	if (graphics){
		snprintf(buffer,sizeof(buffer),"total cpu use  %.2f%%\n",cpu_arr[i][0]);
        write(pipe_cpu, buffer,strlen(buffer));
	    snprintf(buffer, sizeof(buffer),"\033[%dB",i + 1);
        write(pipe_cpu, buffer,strlen(buffer));
        float change = round((cpu_usage1 - cpu_arr[i-1][0])* 100.0)/ 100.0;
		 if (change == 0.00) {
            change = 0.001;
        }
        if (change > 0.00){
            for (int ind =0; ind < change; ind++){
                snprintf(buffer,sizeof(buffer),"|");
                write(pipe_cpu,buffer,strlen(buffer));
            }
            snprintf(buffer,sizeof(buffer)," %.2f%%\n", cpu_usage1);
            write(pipe_cpu, buffer,strlen(buffer));
        }
        else{
            for(int ind = 0; i > change; ind--)
            {
			    snprintf(buffer,sizeof(buffer),"|");
                write(pipe_cpu,buffer,strlen(buffer));
            }
            snprintf(buffer,sizeof(buffer)," %.2f%%\n", cpu_usage1);
            write(pipe_cpu, buffer,strlen(buffer));
        }
    }
    else{
        snprintf(buffer,sizeof(buffer),"total cpu use  %.2f%%\n",cpu_arr[i][0]);
	    write(pipe_cpu,buffer,strlen(buffer));
    }
}