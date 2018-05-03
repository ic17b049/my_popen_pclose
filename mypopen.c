/**
* @file mypopen.c
* Betriebssysteme My Popen File
* Beispiel 2
*
* @author Dominic Schebeck 			<ic17b049@technikum-wien.at>
* @author Thomas  Neugschwandtner 	<ic17b082@technikum-wien.at>
* @author Dominik Rychly 			<ic17b052@technikum-wien.at>
*
* @date 05/03/2018
*
* @version 1.0
*
*/

/*
* ------------------------------------------------------includes----------
*/

#include "mypopen.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

static pid_t pid = -1;
FILE *retpointer = NULL;
	
enum { PIPE_FD_READ, PIPE_FD_WRITE};
FILE *mypopen(const char *command, const char *type){
	
	
	if(type == NULL){
		errno = EINVAL;
		return NULL;
	}
	
	
	if(type[0] != 'r' && type[0] != 'w'){
		errno = EINVAL;
		return NULL;
	}	
	
	if(type[1] != '\0'){
		errno = EINVAL;
		return NULL;
	}
	
	if(pid != -1){
		errno = EAGAIN;
		return NULL;
	}
	
	
	int pipefd[2];

	if (pipe(pipefd) == -1) {
		// errno already set by pipe
		return NULL;
	}

    pid = fork();

	switch(pid) {
		case -1:
			close(pipefd[PIPE_FD_READ]);
			close(pipefd[PIPE_FD_WRITE]);
			errno = EAGAIN;
			return NULL;			
		break;
		
		case 0:
			//child		
			if(type[0] == 'r')
			{
				close(pipefd[PIPE_FD_READ]);
				if(pipefd[PIPE_FD_WRITE] != STDOUT_FILENO)
				{
					if(dup2(pipefd[PIPE_FD_WRITE], STDOUT_FILENO) == -1)
					{
						close(pipefd[PIPE_FD_WRITE]);
						_Exit(EXIT_FAILURE);
					}
					close(pipefd[PIPE_FD_WRITE]);
				}
			}
			
			if(type[0] == 'w')
			{
				close(pipefd[PIPE_FD_WRITE]);
				if(pipefd[PIPE_FD_READ] != STDIN_FILENO)
				{
					if(dup2(pipefd[PIPE_FD_READ], STDIN_FILENO) == -1)
					{
						close(pipefd[PIPE_FD_READ]);
						_Exit(EXIT_FAILURE);
					}
					close(pipefd[PIPE_FD_READ]);
				}
			}			
	
			execl("/bin/sh", "sh", "-c", command, (char*) NULL);
			//reaches only when execl fails
			_Exit(EXIT_FAILURE);
		break;

		default:
			//parent
			if(type[0] == 'r') {
				close(pipefd[PIPE_FD_WRITE]);
				retpointer =  fdopen(pipefd[PIPE_FD_READ], "r");
			}
			if(type[0] == 'w'){
				close(pipefd[PIPE_FD_READ]);
				retpointer =  fdopen(pipefd[PIPE_FD_WRITE], "w");
			}
		break;
	}

	if(retpointer == NULL){

		//reset Vars.
		pid = -1;
		//retpointer = NULL; is condition to reach
		
		if(type[0] == 'r'){
			close(pipefd[PIPE_FD_READ]);
		}
		if(type[0] == 'w'){
			close(pipefd[PIPE_FD_WRITE]);
		}
	}
	return retpointer;
	 
}

int mypclose(FILE *stream){
	
	if (pid == -1){
		errno = ECHILD;
		return -1;
	}
	
	if(retpointer != stream || retpointer == NULL){
		errno = EINVAL;
		return -1;		
	}
	
	if(fclose(retpointer) == EOF){
		
		//reset vars.
		pid = -1;
		retpointer = NULL;

		return -1;
	}
	
	int status;
	int wait_pid;
	
	do{
		wait_pid = waitpid(pid, &status,0);
	}while(wait_pid == -1 && errno == EINTR);

	//reset Vars.
	pid = -1;
	retpointer = NULL;
	
    if (wait_pid == -1){
        errno = ECHILD;
        return -1;
    }

    if (WIFEXITED(status)) {
       return WEXITSTATUS(status);
	}else{
		errno = ECHILD;
		return -1;		   
	}
}
//------------------------------------eof--------------------------------------------------