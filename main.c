/*
 *
 * Last modified : 2017.12.17
 * Hanyang University
 * Computer Science & Engineering
 * Seon Namkung
 *
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int process();
extern void init_cache();

#define MSG 255

int main(int argc, char*argv[],char * envp[]){
  init_cache();
  while(1){
    process();
  }
}
