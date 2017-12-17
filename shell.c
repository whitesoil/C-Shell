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
        char cwd[MSG];              //Current working directory.
        char command[MSG];          //Input command.
        char temp[MSG];             //Save whole command temporarily to judge wether background(&) working or not
        int pid;                    //Save pid of child process
        int status;                 //Get status of child process
        int fd;                     //File descriptor for redirction
        int redir_hash = -1;        //Save the hash # of redirection category. (#OfWords * 10) + ClassfierOfRedirection
        int redir_command;          //Save the ClassfierOfRedirection; redir_hash%10
        int redir_last;             //Save the last index of word on command (It is file name); redir_hash/10
        int command_number;         //# Of command which is classified by "|" or ";"
        int pipe_flag = 0;          //Whether this is pipe
        int p[10][2];               //Pipe
        char ** tokens = malloc(MSG*sizeof(char*)); // tokens(*argv[]) to insert at execvp
        char ** commands = malloc(MSG*sizeof(char*)); //Save commands which is classified by "|" or ";"
        Node * node = (Node *)malloc(sizeof(Node));  //For cache

        /*
        * Get current working directory.
        * Input command.
        */
        getcwd(cwd,MSG);
        printf("%s$",cwd);
        fgets(command,MSG,stdin);
        fflush(stdout);

        /*
        * If command is !! of !#, find the previous command.
        * If not, operate originally.
        */
        if(command[0] == '!' && strlen(command) > 2) {
                if(command[1] == '!') {
                        node = find_Node(1);
                }else{
                        int order = atoi(command+1);
                        node = find_Node(order);
                }
                //Change command to cached command.
                memcpy(command,node->command,MSG);
        }else{
            /*
            *Delete newline and concat NULL.
            *Insert Command to cache.
            */
                int len = strlen(command);
                *(command+(len-1)) = '\0';
                strcpy(node->command,command);
                insert_at_first(node);
        }

        /*
        * Tokenize by '|' or ';'
        * If don't contain '|' or ';', return original command.
        * return # of commands.
        */
        if(index(command,'|') != NULL) {
                command_number = pipe_tokenizer(command,commands);
                pipe_flag = 1;
        }else{
                command_number = semicolon_tokenizer(command,commands);
        }

        //Initiate Pipe
        pipe(p[0]);

        //Loop # of command times
        for(int i =0; i<command_number; i++) {
                pipe(p[i+1]);
                if(commands[i][0] == '!' && strlen(commands[i]) >= 2) {
                        /*
                        * Check one more for "ls -al; !!; !3" case.
                        */
                        if(commands[i][1] == '!') {
                                node = find_Node(2);
                        }else{
                                int order = atoi(commands[i]+1);
                                node = find_Node(order+1);
                        }
                        //Change command to cached command.
                        memcpy(commands[i],node->command,MSG);
                }
                memcpy(temp,commands[i],MSG);
                if(index(commands[i],'>') != NULL || index(commands[i],'<') != NULL) {
                        //In case of redirection.
                        if((redir_hash = redirect_tokenizer(commands[i],tokens)) == -1){
                          perror("Redirection error");
                          exit(1);
                        }
                        redir_command = redir_hash%10;
                        redir_last = redir_hash/10;
                }else{
                        //Normal case.
                        str_tokenizer(commands[i],tokens);
                }
                if(strcmp(tokens[0],"cd") == 0) {
                  /*
                  * If command is cd, just change directory whithout child process.
                  */
                        if(chdir(tokens[1])) {
                                perror("cd error");
                                return 1;
                        }
                }else{
                        if(!(pid=fork())) {
                            //Child Process
                                if(redir_hash != -1) {
                                  //In case of redirction.
                                        umask(0); // Set umast 0;
                                        if(redir_command == 0) { // >
                                                fd = open(tokens[redir_last],O_WRONLY|O_CREAT|O_TRUNC);
                                                chmod(tokens[redir_last],0777);
                                                dup2(fd,STDOUT_FILENO);
                                        }else if(redir_command == 1) { // <
                                                fd = open(tokens[redir_last],O_RDONLY);
                                                chmod(tokens[redir_last],0777);
                                                dup2(fd,STDIN_FILENO);
                                        }else if(redir_command == 2) { // >>
                                                fd = open(tokens[redir_last],O_WRONLY|O_APPEND);
                                                chmod(tokens[redir_last],0777);
                                                dup2(fd,STDOUT_FILENO);
                                        }else if(redir_command == 3) { // >!
                                                fd = open(tokens[redir_last],O_WRONLY|O_TRUNC);
                                                chmod(tokens[redir_last],0777);
                                                dup2(fd,STDOUT_FILENO);
                                        }
                                        if(fd == -1){
                                          perror("open file");
                                          exit(1);
                                        }
                                        close(fd);
                                        tokens[redir_last] = NULL;  // Change file_name to NULL
                                }
                                if(pipe_flag) {
                                  //In case of Pipe
                                        if(i == 0) {
                                          // First command. Deliver output to next process.
                                                dup2(p[i+1][1], STDOUT_FILENO);

                                                close(p[i+1][0]);
                                                close(p[i+1][1]);
                                        }else if(i != command_number-1 ) {
                                          //Next commands. Get input from provious process and Send output to next process.
                                                dup2(p[i][0], STDIN_FILENO);
                                                dup2(p[i+1][1], STDOUT_FILENO);

                                                close(p[i][0]);
                                                close(p[i][1]);
                                                close(p[i+1][0]);
                                                close(p[i+1][1]);
                                        }else if(i == command_number-1) {
                                          //Last process. Only get input from previous process.
                                                dup2(p[i][0], STDIN_FILENO);
                                                close(p[i][0]);
                                                close(p[i][1]);
                                                close(p[i+1][0]);
                                                close(p[i+1][1]);
                                        }else{
                                          //Exception.
                                          close(p[i][0]);
                                          close(p[i][1]);
                                        }
                                }
                                //Exec process.
                                if(execvp(tokens[0],tokens)<0){
                                    perror("exec call");
                                    exit(1);
                                }
                                exit(0);
                        }else{
                          //Parent Process
                                if(pipe_flag) {
                                    //Close Pipe.
                                        close(p[i][0]);
                                        close(p[i][1]);
                                }
                                /*
                                * If work is not background, wait child process.
                                * If background, just pass.
                                */
                                if(index(temp,'&') == NULL) {
                                        waitpid(pid,&status,0);
                                }
                        }
                }

        }
        return 0;
}

/*
* Tokenize command by space and newline and NULL.
* ls -al -> ls , -al
*/
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

/*
* Tokenize command and save to commands.
* Tokenized by ";".
* command1 ; command2 ; more -> command1, command2, more
*/
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

/*
* Tokenize command and save to commands.
* Tokenized by "|".
* command1 | command2 | more -> command1, command2, more
*/
int pipe_tokenizer(char * command,char ** commands){
        char * token;
        char * next_ptr;
        int i = 0;

        token = strtok_r(command,"|",&next_ptr);

        while(token != NULL) {
                commands[i] = token;
                token = strtok_r(NULL,"|",&next_ptr);
                i++;
        }
        return i;
}

/*
* Tokenize command and save to tokens.
* Tokenized by redirections.
* Use hash.
* return % 10 : kind of redirection. 1 : > , 2 : < , 3 : >> , 4 : >!
* return / 10 : # of tokens. To know position of file_name.
*/

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
