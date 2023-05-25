#include "A3.h"


void handle_sigint(int sig) {
    char quit_program;
    printf("sure you want to quit? (y/n) ");
    scanf("%c", &quit_program);
    if (quit_program == 'y' || quit_program == 'Y') {
            exit(0);
    } else {
        return;
        }
}


void handle_sigtstp(int sig) {
	return;
}

pid_t get_user(int user_pipe[2]){
	pipe(user_pipe);
	if(user_pipe<0){
		fprintf(stderr,"%s","Pipe is not working\n");
		exit(1);
	}
    
    pid_t pid = fork();
	if(pid<0){
		fprintf(stderr,"%s","fork is not working\n");
		exit(1);
	}
    if (pid == 0) { 
		signal(SIGINT, SIG_DFL);
        close(user_pipe[0]);
        get_user_info(user_pipe[1]); 
        close(user_pipe[1]); 
        exit(0);
    } else { 
        close(user_pipe[1]); 
	}
	
	return pid;
}

void read_users_from_child(int pipe_num,size_t bytes_read){
	char buffer_u[1024];
	 while ((bytes_read = read(pipe_num, buffer_u, sizeof(buffer_u) - 1)) > 0) {
            buffer_u[bytes_read] = '\0';
        printf("%s", buffer_u);
	  }
	  if( read(pipe_num,buffer_u,sizeof(buffer_u)-1)==-1){
		fprintf(stderr,"%s","Pipe read is not working\n");
		exit(1);
	  }
	close(pipe_num);
}

pid_t get_mem(int pipe_m[2],char memory[][1024],float the_values[][1024],int i, bool graphics,bool sequential,int samples,float prev){
	pipe(pipe_m);
	if(pipe_m<0){
		 fprintf(stderr, "%s", "pipe is not working.\n");   
        exit(1);
	}
   
    pid_t pid_m = fork();
	if( pid_m<0){
		 fprintf(stderr, "%s", "fork is not working.\n");
        exit(1);
	}
    if (pid_m == 0) { // Child process
		signal(SIGINT, SIG_DFL);
        close(pipe_m[0]); // Close read end
        usage_of_memory(pipe_m[1],memory,the_values,i,graphics,sequential,samples,prev); // Send output through the pipe
        close(pipe_m[1]); // Close write end
		
        exit(0);
    } else { // Parent process
        close(pipe_m[1]); // Close write end
	}
	return pid_m;
	}
void read_mem_from_child(int pipe_num,size_t bytes_read){
	char buffer_m[1024];
	 while ((bytes_read = read(pipe_num, buffer_m, sizeof(buffer_m) - 1)) > 0) {
            buffer_m[bytes_read] = '\0';
        printf("%s", buffer_m);
	  }
	   if( read(pipe_num,buffer_m,sizeof(buffer_m)-1)==-1){
		fprintf(stderr,"%s","Pipe read is not working\n");
		exit(1);
	  }
	  close(pipe_num);
	  
}
pid_t get_cpu(int pipe_cpu[2],double cpu_arr[][1024], int i,bool graphics, int tdelay,bool sequential){
	pipe(pipe_cpu);
	if( pipe_cpu<0){
		fprintf(stderr,"%s","Pipe is not working\n");
		exit(1);
	}
    pid_t cpu_pid = fork();
	if(cpu_pid<0){
		fprintf(stderr,"%s","fork is not working\n");
		exit(1);
	}
    if (cpu_pid == 0) { 
		signal(SIGINT, SIG_DFL);
        close(pipe_cpu[0]); 
        cpu_info(pipe_cpu[1],cpu_arr,i, graphics, tdelay,sequential); // Send output through the pipe
        close(pipe_cpu[1]); 
        exit(0);
    } else { 
        close(pipe_cpu[1]); 
		}
	return cpu_pid;
	}
	void read_cpu_from_child(int pipe_num,size_t bytes_read){
	char buffer_m[1024];
	 while ((bytes_read = read(pipe_num, buffer_m, sizeof(buffer_m) - 1)) > 0) {
            buffer_m[bytes_read] = '\0';
        printf("%s", buffer_m);
	  }
	  if( read(pipe_num,buffer_m,sizeof(buffer_m))==-1){
		fprintf(stderr,"%s","Pipe read is not working\n");
		exit(1);
	  }
	 close(pipe_num);
    }

int main ( int argc, char **argv){
	
	
	bool system = false; // for when system flag is used 
	bool user = false; // for when user flag is used 
	bool graphics=false; // for when graphics flag is called 
	bool sequential =false; // for when sequential flag is called 
	int samples= 10; // default value of samples 
	int tdelay= 1; // default value of tdelay 
    signal(SIGINT, handle_sigint);
    signal(SIGTSTP, handle_sigtstp);
    printf("\033[2J\033[H");
	if ( argc==0){
			system=true;
			user=true;
	} 
	for ( int i=0; i<argc;i++){
		  if (argc>1){
	       if (strcmp(argv[i],"--s")==0 || strcmp(argv[i],"--system")==0 ){
				system=true;
			
			}
			else if ( strcmp(argv[i],"--u")==0 ||strcmp(argv[i],"--user")==0){
				user=true;
			}
			else if ( strcmp(argv[i],"--g")==0 ||strcmp(argv[i],"--graphics")==0){
				graphics=true;
			}
			else if (atoi(argv[i])!=0) // for positional arguments
			{
				
				samples = atoi(argv[1]);
				tdelay=atoi(argv[2]);
			}
			else if ( strcmp(argv[i],"--se")==0 ||strcmp(argv[i],"--sequential")==0){
				sequential=true;
			}
			else if  ( strncmp(argv[i],"--samples=N",9)==0){
				char get_cla [30];
				strncpy(get_cla,argv[i],12);
				char *words=strtok(get_cla,"=");
				int c=0;
				
				while ( words!=NULL)
				{
					
					c++;
					if ( c==2){
					samples=atoi(words);
					}
					
					words=strtok(NULL,"=");
				}
			}
			else if  (strncmp(argv[i],"--tdelay=N",7)==0){
				char get_cla [30];
				strncpy(get_cla,argv[i],12);
				char *words=strtok(get_cla,"=");
				int c=0;
				
				while ( words!=NULL)
				{
					
					c++;
					if ( c==2){
					int a =atoi(words);
					if ( a!=0){
						tdelay=a;
					}
					
					}
					
					words=strtok(NULL,"=");
				}
			}
		}
	
	}
   
   char memory[samples][1024];
   float the_prev_values[samples][1024];
   double cpu_arr[samples][1024];
   float the_values[samples][1024];
   printf("Nbr of samples: %d -- every %d secs\n",samples,tdelay);
   total_usage();
   
   // for loop upto samples then pass in the iteration i
    for ( int i=0; i<samples; i++){
	int user_pipe[2],cpu_pipe[2],pipe_m[2];
	pid_t pid_u;
	pid_t pid_m;
	pid_t cpu;
	if(system && !user&& !sequential){
	
		size_t read_cpu,read_me;
	    memset(&read_cpu, 0, sizeof(read_cpu));
        memset(&read_me, 0, sizeof(read_me));
		get_previous(the_prev_values,i);
		pid_m=get_mem(pipe_m,memory,the_values,i, graphics,sequential,samples,the_prev_values[i-1][0]);
		read_mem_from_child(pipe_m[0],read_me);
        waitpid(pid_m,NULL,0);
		printf("\033[H");
        printf("\033[%dB",samples+4);
		core_numbers();
		pid_u=get_cpu(cpu_pipe,cpu_arr,i,graphics,tdelay,sequential);
		read_cpu_from_child(cpu_pipe[0],read_cpu);
		waitpid(cpu,NULL,0);
	}
   else if(user && !system){
       pid_u=get_user(user_pipe);
	   ssize_t read_users;
	   memset(&read_users, 0, sizeof(read_users));
	   printf("\033[H");
       printf("\033[%dB",3);
	   read_users_from_child(user_pipe[0],read_users);
       waitpid(pid_u,NULL,0);
       sleep(tdelay);
    }
	else if(sequential){
	size_t read_users,read_cpu,read_me;
	memset(&read_users, 0, sizeof(read_users));
    memset(&read_cpu, 0, sizeof(read_cpu));
    memset(&read_me, 0, sizeof(read_me)); 
	get_previous(the_prev_values,i); 
    pid_m=get_mem(pipe_m,memory,the_values,i,graphics,sequential,samples,the_prev_values[i-1][0]);
	read_mem_from_child(pipe_m[0],read_me);
     waitpid(pid_m,NULL,0);
	if(!system || user){
	
	    pid_u=get_user(user_pipe);
        read_users_from_child(user_pipe[0],read_users);
        waitpid(pid_u,NULL,0);
    }
		core_numbers();
		cpu=get_cpu(cpu_pipe,cpu_arr,i,graphics,tdelay,sequential);
	    read_cpu_from_child(cpu_pipe[0],read_cpu);
        waitpid(cpu,NULL,0);
		get_system_info();
		
	   
	   
	}
    
	else{
		size_t read_users,read_cpu,read_me;
		memset(&read_users, 0, sizeof(read_users));
        memset(&read_cpu, 0, sizeof(read_cpu));
        memset(&read_me, 0, sizeof(read_me));
		get_previous(the_prev_values,i);
        pid_m=get_mem(pipe_m,memory,the_values,i, graphics,sequential,samples,the_prev_values[i-1][0]);
		read_mem_from_child(pipe_m[0],read_me);
        waitpid(pid_m,NULL,0);
		
		pid_u=get_user(user_pipe);
		
		printf("\033[H");
        printf("\033[%dB",samples+4);
		read_users_from_child(user_pipe[0],read_users);
        waitpid(pid_u,NULL,0);
		core_numbers();
		cpu=get_cpu(cpu_pipe,cpu_arr,i,graphics,tdelay,sequential);
	    read_cpu_from_child(cpu_pipe[0],read_cpu);
        waitpid(cpu,NULL,0);	
        
		
	    
	    
	}
	

}
if(!sequential){
get_system_info();
}

return 0;
}
   

