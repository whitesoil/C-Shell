/*
 * 2013042776 남궁선
 * Last modified : 2017 / 12 / 17
 */
#include "shell.h"

int process(){
        char cwd[MSG];
        char command[MSG];
        char temp[MSG];
        int pid;
        int status;
        char ** tokens = malloc(MSG*sizeof(char*));
        char ** commands = malloc(MSG*sizeof(char*));

        getcwd(cwd,MSG);
        printf("%s$",cwd);
        fgets(command,MSG,stdin);
        int len = strlen(command);
        *(command+(len-1)) = '\0';

        int semicolon_number = semicolon_tokenizer(command,commands);
        for(int i =0; i<semicolon_number; i++) {
                memcpy(temp,commands[i],MSG);
                str_tokenizer(commands[i],tokens);
                if(strcmp(tokens[0],"cd") == 0) {
                        if(chdir(tokens[1])) {
                                perror("cd error");
                                return 1;
                        }
                }else{
                  if(!(pid=fork())) {
                          execvp(tokens[0],tokens);
                          exit(0);
                  }else{
                          if(bg_checker(temp) == NULL) {
                                  waitpid(pid,&status,0);
                          }
                  }
                }

        }
        return 0;
}

void str_tokenizer(char * command,char ** tokens){
        char * token;
        char * next_ptr;
        int i = 0;

        token = strtok_r(command," &\r\n\t",&next_ptr);

        while(token != NULL) {
                tokens[i] = token;
                token = strtok_r(NULL," &\r\n\t\0",&next_ptr);
                i++;
        }
        tokens[i] = NULL;
}

int semicolon_tokenizer(char * command,char ** commands){
        char * token;
        char * next_ptr;
        int i = 0;

        token = strtok_r(command,";",&next_ptr);

        while(token != NULL) {
                commands[i] = token;
                token = strtok_r(NULL,";",&next_ptr);
                i++;
        }
        return i;
}


void * bg_checker(char * command){
        return index(command,'&');
}

int semicolon_checker(char * command){
        int i;
        int number = 0;
        while(command[i] != '\0') {
                if (command[i] == ';') {
                        number++;
                }
                i++;
        }
        return i;
}

int pipe_checker(char * command){
        int i;
        int number = 0;
        while(command[i] != '\0') {
                if (command[i] == '|') {
                        number++;
                }
                i++;
        }
        return i;
}
