/*
 * Header file for circular queue of HistoryListItems.
 */
#ifndef __HISTQUEUE_H__
#define __HISTQUEUE_H__

#include "parse_args.h"

#define MAXHIST 10   // max number of commands in history list

/*
 * A struct to keep information one command in the history of 
 * command executed
 */
struct HistoryEntry {              
    unsigned int cmd_num;
    char cmdline[MAXLINE]; // command line for this process
};

// You can use "HistoryEntry" instead of "struct HistoryEntry"
typedef struct HistoryEntry HistoryEntry;

void print_queue();
void add_queue(char *arg, int cmd_num);
char *peek_queue();
char *pastCmd(char *cmdline);
#endif
