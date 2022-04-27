#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char line[1024];
char *argv[64];
char *command1[64];
char *command2[64];

//removing spaces into multiple lines
void parse(){  
     int i = 0;

     char * token = strtok(line, " ");
     while(token != NULL){
          argv[i++] = token;
          token = strtok(NULL, " ");
     }        
}

//so we can run multiple commands
void parseMult(){
     command1[0] = argv[1];
}

//change dir
void disDir(){
     char cwd[1024];
     getcwd(cwd, sizeof(cwd));
     printf("\nDir: %s", cwd);
}

//restart args
void clearArgs(){
     for(int i = 0; i < 64; i++){
          argv[i] = NULL;
          command1[i] = NULL;
          command2[i] = NULL;
     }
}

//removing semi colon so arg can be run
void removeSemi(){
     char *token;
     token = strstr(argv[0], ";");
     *token = '\0';
}

//executing one arg
void executeArg(){
     pid_t pid;
     int status;
     
     if((pid = fork()) < 0){
          printf("Could not execute \n");
     }
     else if(pid == 0){
          if(execvp(*argv, argv) < 0 ){
               printf("Command failed\n");
          }
     }
     else{
          while (wait(&status) != pid);
     }
}

//executing two arguments at the same time 
void executeArg2(){
     pid_t pid;
     int status;
     
     if((pid = fork()) < 0){
          printf("Could not execute \n");
     }
     else if(pid == 0){
          if(execvp(*command1, command1) < 0 ){
               printf("Command failed\n");
          }
     }
     else{
          while (wait(&status) != pid);
     }
}

//running both commands by forking
void piped(){
     int pipefd[2];
     pid_t p1, p2;

     command1[0] = argv[0];
     command2[0] = argv[2];

     if(pipe(pipefd) < 0){
          printf("Could not initiliaze pipe");
     }
     p1 = fork();
     if(p1< 0){
          printf("Could not fork");
     }
     if (p1 == 0) {//child 1
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
  
        if (execvp(command1[0], command1) < 0) {
            printf("\nCould not execute command 1..");
            
        }
    } else {//child 2
        
        p2 = fork();
  
        if (p2 < 0) {
            printf("\nCould not fork");
            return;
        }
  
        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(command2[0], command2) < 0) {
                printf("\nCould not execute command 2..");
          
            }
        } 
    }
}

void helpcomm(){
     printf("Built in commands are: \n");
     printf("cd\n");
     printf("help\n");
     printf("exit\n");
     printf("Can also handle piping and multiple commands by ;\n");
}


//process command
void execComm(){
     char ex[] = "exit";
     char help[] = "help";
     char cd[] = "cd";
     char pip[] = "|";


     if(argv[1] != NULL){
          if(strcmp(argv[0], ex) == 0){
               _Exit(0);
          }
          else if(strcmp(argv[0], help) == 0){
               //printf("opening help\n");
               helpcomm();
          }
          else if(strcmp(argv[0], cd) == 0){
               chdir(argv[1]);
          }
          else if(strstr(argv[0], ";") != NULL){
               //printf("multiple commands");
               removeSemi();
               parse();
               parseMult();
               executeArg();
               executeArg2();
          }
          else if(strcmp(argv[1], pip) == 0){
               //printf("piping");
               piped();
               //printf(command1[0]);
               //printf(command2[0]);
          }
          else{
               parse();
               executeArg();
          }
     }
     else{
          if(strcmp(argv[0], ex) == 0){
               _Exit(0);
          }
          else if(strcmp(argv[0], help) == 0){
               //printf("opening help\n");
               helpcomm();
          }
          else if(strcmp(argv[0], cd) == 0){
               chdir(argv[1]);
          }
          else if(strstr(argv[0], ";") != NULL){
               //printf("multiple commands");
               removeSemi();
               parse();
               parseMult();
               executeArg();
               executeArg2();
          }
          else{
               parse();
               executeArg();
          }
     }
}



/*
displayArgDebug(){
     for (int i = 0; i < 3; ++i){
               printf(argv[i]);
               printf("\n");
          }
}
*/
int main() {
     
     //main loop
     while(1){
          disDir();
          printf("\n>> ");
          gets(line);
          printf("\n");
          parse();

          execComm();

          //displayArgDebug();
          clearArgs();
      
   }
}