#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(void){
  umask(0);
  int status;
  int fd = open("test.txt",O_RDWR|O_CREAT|O_TRUNC);
  chmod("test.txt",0777);
  printf("asdf:");
  fflush(stdout);
  dup2(fd,STDOUT_FILENO);
  int pid;

  if(!(pid=fork())) {
          close(fd);

          execlp("ls","ls",(char*)0);
          exit(1);
      } else {
        close(fd);
        waitpid(pid,&status,0);
      }
}
