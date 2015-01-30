#include <iostream>
#include <string>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <pwd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include "rshell.h"


using namespace std;

#define MAX_LEN 10000
char command[MAX_LEN];

void command_handling(const char* cmd){ // handling the command
    
    vector<string> part;
    char* currentCommand;
    char* argv[MAX_LEN];
    
    bool operateResult = false;         // store the status after command execution,
                                        // false for error occured, true for executing correctly
    
    bool jumpflag = false;              // sotre the status if the next command should be execute after the logical operating,
                                        // false for executing the next command, true for not executing
    
    char *sep = (char*)" ";             // strtok arguement, separate the command when meet ' '
    char *word, *brkt;                  // strtok arguement
    char *partable[MAX_LEN];            // store command and arguement equally
    
    int i = 0;
    for (word = strtok_r(command, sep, &brkt); word; word = strtok_r(NULL, sep, &brkt)){
        // store command and argument one by one in vector<stirng> part
        string a = word;
        partable[i] = word;
        i++;
        part.push_back(a);
    }
    
    //    for(int i = 0; i < part.size(); i++)
    //        cout << "part["<< i << "]: " << part.at(i) << endl;
    
    for(int i = 0; i < part.size(); i++){
        
        if(part.at(i) == "&&"){
            if(operateResult == false)  // if the command result before "&&" is false, skip the next command after "&&"
                jumpflag = true;
            else                        // if the command result before "&&" is true, execute the next command after "&&"
                jumpflag = false;
        }
        else if(part.at(i) == "||"){
            if(operateResult == true)   // if the command result before "||" is true, skip the next command after "||"
                jumpflag = true;
            else                        // if the command result before "||" is false, execute the next command after "||"
                jumpflag = false;
        }
        else if(part.at(i)==";"){       // turn the jumpflag to false to make sure the command after ";" being executed
            jumpflag = false;
        }
        else{                           // command executing
            currentCommand = partable[i];
            argv[0] = currentCommand;
            int j,argv_size;
            for(i=i+1,j=1; i < part.size(); i++){  // store the arguements in argv[]
                if(part.at(i)=="&&" || part.at(i)=="||" || part.at(i)==";")
                    break;
                else{
                    argv[j] = partable[i];
                    j++;
                }
            }
            argv_size = j;
            if(jumpflag == true){       // initialize the jumpflag
                jumpflag = false;
            }
            else{
                // operate the command
                int condition=0;
                int pid = fork();       // child return 0 , parent return pid, fail return -1
                if(pid == -1){          // system call errer check
                    perror("fork fail");
                    operateResult = false;
                    exit(5);
                }
                else if (pid == 0){
                    if (execvp(currentCommand,argv) != 0){ // system call error check
                        perror("error in execvp");
                        operateResult = false;              // if command executing failed, turn the operating result to false
                        exit(6);
                    }
                }
                else{
                    operateResult = true;
                    if(-1 == (waitpid(pid, &condition, 0)) && (errno != EINTR)){ // system call error check
                        perror("waitpid error");
                        operateResult = false;
                    }
                    // cout << "WEXITSTATUS(condition): " << WEXITSTATUS(condition) << endl;
                    if(0 != WEXITSTATUS(condition)){ // exit properly when condition is 0
                        operateResult = false;  // if command executing failed, turn the operating result to false
                        cout << "error message: child process abortion" << endl;
                    }
                }
            }
            i--;
            for(int i = 0; i < argv_size; i++)
                argv[i]=NULL;
        }
    }
}

void prehandling_command()
{
    cin.getline(command, MAX_LEN);      // get the command
    int len = (unsigned int)strlen(command);
    
    for(int i = 0; i < len; i++)        // handling the comment signal "#"
        if(command[i] == '#'){
            command[i] = '\0';
            break;
        }
    
    for(int i = 0; i < len; i++){       // format input commands like "command_a;command_b" as "command_a ; command_b"
        if(command[i] == ';'){
            len += 2;
            for(int j = len + 1; j >i+2; j--)
                command[j] = command[j-2];
            command[i]=' ';
            command[i+1]=';';
            command[i+2]=' ';
            i=i+2;
        }
    }
}

void get_command()
{
    while(1){
        type_prompt();
        prehandling_command();
        if(0 == strncmp(command, "exit", sizeof(command))){
            break;
        }
        command_handling(command);
    }
}

int main(){
    get_command();
    return 0;
}
