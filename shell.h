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
#ifndef __MAIN__
#define __MAIN__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int process();
void str_tokenizer(char *,char ** );
int semicolon_tokenizer(char *,char **);
int pipe_tokenizer(char *,char **);
int redirect_tokenizer(char *,char ** );

#define MSG 256

#endif
