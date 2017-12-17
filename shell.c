/*
 *
 * Last modified : 2017.12.17
 * Hanyang University
 * Computer Science & Engineering
 * Seon Namkung
 *
 * C Linux Shell
 *
 */
#include "shell.h"
#include "cache.h"

int process(){
        char cwd[MSG];
        char command[MSG];
        char temp[MSG];
        int pid;
        int status;
        int fd;
        int redir_hash = -1;
        int redir_command;
        int redir_last;
        char ** tokens = malloc(MSG*sizeof(char*));
        char ** commands = malloc(MSG*sizeof(char*));
        Node * node = (Node *)malloc(sizeof(Node));

        getcwd(cwd,MSG);
        printf("%s$",cwd);
        fgets(command,MSG,stdin);
        fflush(stdout);
        if(command[0] == '!' && strlen(command) > 2) {
                if(command[1] == '!') {
                        node = find_Node(1);
                }else{
                        int order = atoi(command+1);
                        node = find_Node(order);
                }
                memcpy(command,node->command,MSG);
        }else{
                int len = strlen(command);
                *(command+(len-1)) = '\0';
                strcpy(node->command,command);
                insert_at_first(node);
        }

        int semicolon_number = semicolon_tokenizer(command,commands);
        for(int i =0; i<semicolon_number; i++) {
                if(commands[i][0] == '!' && strlen(commands[i]) >= 2) {
                        if(commands[i][1] == '!') {
                                node = find_Node(2);
                        }else{
                                int order = atoi(commands[i]+1);
                                node = find_Node(order+1);
                        }
                        memcpy(commands[i],node->command,MSG);
                }
                memcpy(temp,commands[i],MSG);
                if(index(commands[i],'>') != NULL || index(commands[i],'<') != NULL) {
                        redir_hash = redirect_tokenizer(commands[i],tokens);
                        redir_command = redir_hash%10;
                        // printf("command : %d\n",redir_command);
                        redir_last = redir_hash/10;
                        // printf("redirect : %s,%s,%s,%s \n",tokens[0],tokens[1],tokens[2],tokens[3]);
                }else{
                        str_tokenizer(commands[i],tokens);
                }
                if(strcmp(tokens[0],"cd") == 0) {
                        if(chdir(tokens[1])) {
                                perror("cd error");
                                return 1;
                        }
                }else{
                        if(!(pid=fork())) {
                                if(redir_hash != -1) {
                                        umask(0);
                                        if(redir_command == 0) {
                                                fd = open(tokens[redir_last],O_WRONLY|O_CREAT|O_TRUNC);
                                                chmod(tokens[redir_last],0777);
                                                dup2(fd,STDOUT_FILENO);
                                        }else if(redir_command == 1) {
                                                fd = open(tokens[redir_last],O_RDONLY);
                                                chmod(tokens[redir_last],0777);
                                                dup2(fd,STDIN_FILENO);
                                        }else if(redir_command == 2) {
                                                fd = open(tokens[redir_last],O_WRONLY|O_APPEND);
                                                chmod(tokens[redir_last],0777);
                                                dup2(fd,STDOUT_FILENO);
                                        }else if(redir_command == 3) {
                                                fd = open(tokens[redir_last],O_WRONLY|O_TRUNC);
                                                chmod(tokens[redir_last],0777);
                                                dup2(fd,STDOUT_FILENO);
                                        }
                                        close(fd);
                                        tokens[redir_last] = NULL;
                                }
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

        token = strtok_r(command," &\r\n\t\0",&next_ptr);

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

int redirect_tokenizer(char * command,char ** tokens){
        char * token;
        char * next_ptr;
        int i = 0;

        if(strstr(command,">>") != NULL) {
                token = strtok_r(command," >>",&next_ptr);

                while(token != NULL) {
                        tokens[i] = token;
                        token = strtok_r(NULL," >>",&next_ptr);
                        i++;
                }
                tokens[i] = NULL;
                i--;
                return (i*10+2);
        }else if(strstr(command,">!") != NULL) {
                token = strtok_r(command," >!",&next_ptr);

                while(token != NULL) {
                        tokens[i] = token;
                        token = strtok_r(NULL," >!",&next_ptr);
                        i++;
                }
                tokens[i] = NULL;
                i--;
                return (i*10+3);
        }else if(strstr(command,">") != NULL) {
                token = strtok_r(command," >",&next_ptr);

                while(token != NULL) {
                        tokens[i] = token;
                        token = strtok_r(NULL," >",&next_ptr);
                        i++;
                }
                tokens[i] = NULL;
                i--;
                return (i*10);
        }else if(strstr(command,"<") != NULL) {
                token = strtok_r(command," <",&next_ptr);

                while(token != NULL) {
                        tokens[i] = token;
                        token = strtok_r(NULL," <",&next_ptr);
                        i++;
                }
                tokens[i] = NULL;
                i--;
                return (i*10+1);
        }else{
                return -1;
        }
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

void* redirect_checker(char * command){
        return index(command,'>');
}
