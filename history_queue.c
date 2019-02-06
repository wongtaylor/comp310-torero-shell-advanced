/*
 * The Tiny Torero Shell (TTSH)
 * Names: Taylor Wong and Cecilia Barnhill
 * Hours: 4 
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "history_queue.h"

// global variables: add only globals for history list state
//                   all other variables should be allocated on the stack
// static: means these variables are only in scope in this .c module
static HistoryEntry history[MAXHIST]; 
	
static int queue_start = 0;
static int queue_next = 0;
static int queue_size = 0;

/**
 * Adds the command line to the history queue
 *
 * @param cmdline The command line entered by user
 * @param cmd_num The count of command lines entered
 */
void add_queue(char *cmdline, int cmd_num)
{
	if(queue_next < MAXHIST)
	{
		history[queue_next].cmd_num = cmd_num;
		strncpy(history[queue_next].cmdline, cmdline, MAXLINE);
		queue_next++;
		queue_size++;
	}
	else
	{
		int next_mod = queue_next % MAXHIST;
		if(queue_start == next_mod){
			queue_start++;
			queue_start = queue_start % MAXHIST;
		}
		history[next_mod].cmd_num = cmd_num;
		strncpy(history[next_mod].cmdline, cmdline, MAXLINE);
	}
}

/**
 * Prints the most recent 10 commands
 */
void print_queue()
{
	int index = queue_start;
	int count = 0;
	while((index < queue_size) & (count < MAXHIST))
	{
		if(index == MAXHIST)
		{	
			index = 0;
		}
		printf("%d \t %s \n", (history[index]).cmd_num, (history[index]).cmdline);	
		count++;
		index++;
	}
}

/**
 * Advanced Component: finds past command to run
 *
 * @param cmdline The command line entered by user
 * @return The command line of the past command to be run again
 */
char *pastCmd(char *cmdline)
{
	strtok(cmdline, "!");
	char *num = strtok(NULL, " ");
	unsigned int cmd_num = strtol(num, NULL, 10);
	for(int i = 0; i < MAXHIST; i++)
	{
		if(cmd_num == history[i].cmd_num)
		{
			cmdline = history[i].cmdline;
		}
		else
		{
			printf("command not found in history \n");
		}
	}
	return cmdline;
}
