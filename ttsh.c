/*
 * The Tiny Torero Shell (TTSH)
 * Names: Taylor Wong and Cecilia Barnhill
 * Hours: 12
 * Project Bucks: 0
 */

// NOTE: all new includes should go after the following #define
#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <signal.h>
#include "parse_args.h"
#include "history_queue.h"

void child_handler(__attribute__ ((unused)) int sig);
int builtin_cmd(char **argv);
void execute_cmd(char **argv, int bg);
extern char *pastCmd(char *cmdline);

int main() { 
	// Handle SIGCHILD signal by calling child_handler signal instead
	struct sigaction act;
	act.sa_handler = child_handler;
	act.sa_flags = SA_NOCLDSTOP;
	sigaction(SIGCHLD, &act, NULL);
	int count = 1;

	while(1) {
		fprintf(stdout, "ttsh> ");  // print the shell prompt
		fflush(stdout);
		// read in the next command entered by the user
		char cmdline[MAXLINE];
		if ((fgets(cmdline, MAXLINE, stdin) == NULL)
				&& ferror(stdin)) {
			clearerr(stdin);
			continue;
		}
		// check to see if you reached the end of the file
		if (feof(stdin)) {
			fflush(stdout);
			exit(0);
		}
		// TODO: remove this line after you are done testing/debugging.
		fprintf(stdout, "DEBUG: %s\n", cmdline);
		char *argv[MAXARGS];
		// parseArguments returns non-zero (true) if the command line includes & at the end
		// indicating that the command should be run in the background
		int bg = parseArguments(cmdline, argv);
		//execute command if not a built in command
		int bi = 0;
		if((bi = builtin_cmd(argv)) == 2) // 0 returned for not a built in command
			strncpy(cmdline, pastCmd(cmdline), MAXLINE);
		else if(bi == 0)
			execute_cmd(argv, bg); 
		//add the command to the history queue and increment count
		add_queue(cmdline,count);
		count++;
		}	
	return 0;
}
void child_handler(__attribute__ ((unused)) int sig){
	pid_t pid;
	int status;
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
	{
		continue;
	}	
}

/**
 * Checks if the command is built in or not.
 *
 * @param argv The array of parsed inputs to the command line
 * @return Returns an int corresponding to the type of command entered (0 for
 * not built in, 1 or 2 for built in)
 */
int builtin_cmd(char **argv){
	if((strcmp(argv[0], "exit")) == 0){
		exit(0);
	}
	else if((strcmp(argv[0], "history")) == 0){
		print_queue();
		return 1;
	}
	else if((strncmp(argv[0], "!", 1)) == 0)
	{
		return 2;
	}
	else if ((strcmp(argv[0], "cd")) == 0){
		if (argv[1] == NULL)
			chdir(getenv("HOME"));
		else{
			if (chdir(argv[1]) == -1)
				printf("No such directory exists! \n");
		}
		return 0;
	}
	else
		return 0;    // not a built in command
}

/**
 * Executes commands that are not built in
 *
 * @param argv The array of parsed inputs to the command line
 * @param bg Integer for background or foreground run
 */
void execute_cmd(char **argv, int bg){
	pid_t pid, wpid;
	if((pid = fork()) == 0){ // child runs user job
		if(execvp(argv[0], argv)){ // <0
			printf("%s: Command not found. \n", argv[0]);
			exit(0);
		}
	}
	// parent process
	if(bg == 0){ // foreground
		wpid = waitpid(pid, NULL, 0);
		printf("%d \n", wpid);
			//int status;
			//while(wpid < 0)
			//	wpid = waitpid(pid, &status, WUNTRACED);
	}
	//else // background
	//	wpid = waitpid(pid, NULL, WNOHANG);
}
/*
void NumCmd(char **argv, int *count){
	char cmdline[strlen(argv[0])];
	// get rid of ! by shifting chars
	int i;
	for (i = 0; i < (int) strlen(argv[0]); i++){
		cmdline[i] = argv[0][i+1];
	}	
	printf("cmdline= %s\n", cmdline);
	unsigned int num = strtol(cmdline, NULL, 0);
	
	if(strcmp(
}
*/
