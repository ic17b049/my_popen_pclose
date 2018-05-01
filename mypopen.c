#include <unistd.h>
#include <stdio.h>
#include "mypopen.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>


// 0	OK
// 1	OK
// 2	OK
// 3	OK
// 4	OK
// 5	OK
// 6	OK
// 7
// 8
// 9	OK
// 10
// 11
// 12	OK
// 13	OK
// 14	OK
// 15	OK
// 16	OK
// 17 	OK
// 18
// 19
// 20	OK
// 21
// 22	OK
// 23
// 24
// 25 
// 26
// 27
// 28
// 29
// 30

static pid_t pid = -1;
static int popenrunning = 0;
FILE *retpointer = NULL;
	
enum { PIPE_FD_READ, PIPE_FD_WRITE};
FILE *mypopen(const char *command, const char *type){
	
	
	if(type == NULL){
		errno = EINVAL;
		return NULL;
	}
	
	if(strlen(type) != 1){
		errno = EINVAL;
		return NULL;		
	}
	
	if(type[0] != 'r' && type[0] != 'w' ){
		errno = EINVAL;
		return NULL;
	}	
	
	
	if(popenrunning == 1){
		errno = EAGAIN;
		return NULL;
	}else{
		popenrunning = 1;
	}
	
	
	int pipefd[2];

	
	pipe(pipefd);
	
    pid = fork();
	
	if(pid == -1){
		close(pipefd[PIPE_FD_READ]);
        close(pipefd[PIPE_FD_WRITE]);
		errno = EAGAIN;
		return NULL;
	}
		
	if(pid == 0){
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
		execl("/bin/sh", "sh", "-c", command, (char*) NULL);
		
	}else{
		if(type[0] == 'r') {
			close(pipefd[PIPE_FD_WRITE]);
			retpointer =  fdopen(pipefd[PIPE_FD_READ], "r");
		}
		if(type[0] == 'w'){
			close(pipefd[PIPE_FD_READ]);
			retpointer =  fdopen(pipefd[PIPE_FD_WRITE], "w");
		}
	}

	if(retpointer == NULL){
		pid = -1;
		popenrunning = 0;
		
		if(type[0] == 'r') close(pipefd[PIPE_FD_READ]);
		else if(type[0] == 'w') close(pipefd[PIPE_FD_WRITE]);
	
		else return NULL;
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
	
	
	int status;
    pid_t wait_pid = wait(&status);
	
	//reset Vars.
	popenrunning = 0;
	pid = -1;
	
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