/*
* 2013042776 컴퓨터공학과 남궁선
* Last modified : 2017/12/16 Sat
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern int process();

#define MSG 255

int main(int argc, char*argv[],char * envp[]){
  while(1){
    process();
  }
}
