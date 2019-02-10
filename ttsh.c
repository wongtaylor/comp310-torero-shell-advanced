/*
 * COMP 310 Operating Systems
 * Project 1: Torero Shell
 * Names: Taylor Wong and Mia Kim
 *
 * TODO Basic commands
 * background/foreground
 * CD and EXIT must work
 * Pipes or IO redirection - one of the two by wednesday
 */

#define _XOPEN_SOURCE 600
#define PATH_MAX 50

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include "parse_args.h"
#include "history_queue.h"
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

void child_handler(__attribute__ ((unused)) int sig);
int builtin_cmd(char **argv);
void execute_cmd(char **argv, int bg);
extern char *pastCmd(char *cmdline);
void printList(char *option);
char* formatTime(struct stat attr);

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
		//fprintf(stdout, "DEBUG: %s\n", cmdline);
		char *argv[MAXARGS];
		// parseArguments returns non-zero (true) if the command line includes & at the end
		// indicating that the command should be run in the background
		int bg = parseArguments(cmdline, argv);
		//execute command if not a built in command
		int bi = 0;
		if((bi = builtin_cmd(argv)) == 2) // 0 returned for not a built in command
		{
			printf("called builtin func \n");
			strncpy(cmdline, pastCmd(cmdline), MAXLINE);
		}
		else if(bi == 0)
		{
			printf("about to execute command \n");
			execute_cmd(argv, bg); 
			printf("executed cmd \n");
		}
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
	else if((strcmp(argv[0], "!!")) == 0)
	{
		char *lastCmd = peek_queue();
		printf("In builtin function %s \n", lastCmd);
		return 1; //builtin_cmd(lastCmd);
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
	else if(strcmp(argv[0], "ls") == 0)
	{
		printList(argv[0]);
		return 1;
	}
	else if(strcmp(argv[0], "ls -al") == 0)
	{
		printList(argv[0]);
		return 1;
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
 * ls command - lists the files within the current directory
 *
 */
void printList(char *option)
{
	printf("in Print List func\n"); 
	//	char currentDir[PATH_MAX];
	char *currentDir = NULL;	
	currentDir = getenv("PWD");
	if(NULL == currentDir) 
	{
		printf("ERROR couldn't get working directory\n");	
	}
	else
	{
		printf("Current directory %s\n", currentDir);
		DIR *myDirectory;
		struct dirent *dirEntry;
		myDirectory = opendir(currentDir);
		if(myDirectory != NULL)
		{
			struct stat attr;
			char *time = NULL;
			while((dirEntry = readdir(myDirectory)) != NULL)
			{
				if(strcmp(option, "ls") == 0) 
				{
					printf("%s\t", dirEntry->d_name);
				}
				else
				{
					stat(dirEntry->d_name, &attr);
					time = ctime(&attr.st_mtime);
					//time = formatTime(attr)
					printf("%o %lu %s %s %ld %s %s\n", attr.st_mode, attr.st_ino, (getpwuid(attr.st_uid))->pw_name, (getgrgid(attr.st_gid))->gr_name, attr.st_size, time, dirEntry->d_name);
				}
			}
		}
		closedir(myDirectory);
	}
}

char* formatTime(struct stat attr)
{
	char *timeStamp = NULL;
	char *time = NULL;
	char actualTime[50], *token, *token2;
	char delim[] = " ";

	timeStamp = ctime(&attr.st_mtime);
	token = strtok(timeStamp, delim); // weekday
	token = strtok(NULL, delim); // month
	strcat(time, token);
	strcat(time, " ");
	token = strtok(NULL, delim); // day #
	strcat(time, token);
	strcat(time, " ");
	token = strtok(NULL, delim); // time
	token2 = strtok(token, ":"); // hour
	strcat(actualTime, token2);
	strcat(actualTime, ":");
	token2 = strtok(NULL, ":"); // minutes
	strcat(actualTime, token2);
	strcat(time, actualTime);

	printf("TIME IN FORMAT TIME FUNC %s\n", time);
	return time;
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
