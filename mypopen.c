#include <unistd.h>
#include <stdio.h>
#include "mypopen.h"
#include <sys/types.h>
#include <sys/wait.h>




enum { PIPE_FD_READ, PIPE_FD_WRITE};

FILE *mypopen(const char *command, const char *type){
	
	int pipefd[2];
	pipe(pipefd);
	
	//pipefd[0] Read
	//pipefd[1] Write

    pid_t pid = fork();

	if(pid == 0){
		
		sleep(1);
		//child
		if(type[0] == 'r') {
			close(STDOUT_FILENO);
			close(pipefd[PIPE_FD_READ]);
			dup2(pipefd[PIPE_FD_WRITE], STDOUT_FILENO);
		}
		if(type[0] == 'w'){
			close(STDIN_FILENO);
			close(pipefd[PIPE_FD_WRITE]);
			dup2(pipefd[PIPE_FD_READ] , STDIN_FILENO );
		}
		
		execl("/bin/sh", "sh", "-c", "ls -l", (char*) NULL);
		
	}else{
		char buf;
		int statusPtr;
		
		pid = wait(&statusPtr);
		printf("child finished\n");
		printf("ParentPrint\n");
		if(type[0] == 'r') {
			close(pipefd[PIPE_FD_WRITE]);
			while (read(pipefd[PIPE_FD_READ], &buf, 1) > 0)
                   printf("%c",buf);
		}
		if(type[0] == 'w'){

		}
		printf("ParentPrint FIN\n");
	}
	 
}

int mypclose(FILE *stream){
	
}