/*
 *
 * Last modified : 2017.12.17
 * Hanyang University
 * Computer Science & Engineering
 * Seon Namkung
 *
 * Header of Shell
 *
 */
#ifndef __SHELL__
#define __SHELL__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MSG 256
#define CACHE 256

typedef struct node {
        struct node * next;
        char command[CACHE];
}Node;

int process();
void str_tokenizer(char *,char ** );
int semicolon_tokenizer(char *,char **);
int pipe_tokenizer(char *,char **);
int redirect_tokenizer(char *,char ** );

extern void init_cache();
extern void insert_at_first(Node *);
extern Node * delete_at_last();
extern void print_cache();
extern Node * find_Node(int);

#endif
