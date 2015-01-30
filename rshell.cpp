#include <iostream>
#include <string>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include "type_prompt.h"
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
    
    // separate the command string with small parts store in vector<string> param
    char *sep = (char*)" ";             // strtok arguement, separate the command when meet ' '
    char *word, *brkt;                  // strtok arguement
    char *partable[MAX_LEN];            // store command and arguement one by one
    
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
                
                int pid = fork();       // child return 0 , parent return pid, fail return -1
                if(pid == -1){          // system call errer check
                    perror("fork fail");
                    operateResult = false;
                    exit(0);
                }
                else if (pid == 0){
                    if (execvp(currentCommand,argv) != 0){ // system call error check
                        perror("error in execvp");
                        operateResult = false;              // if command executing failed, turn the operating result to false
                        exit(0);
                    }
                }
                else{
                    operateResult = true;
                    if(-1 == (waitpid(pid, NULL, 0)) && (errno != EINTR)){ // system call error check
                        perror("wiatpid error");
                        exit(0);
                    }
                    int condition;
                    if(WIFSIGNALED(condition)){ // system call error check
                        operateResult = false;  // if command executing failed, turn the operating result to false
                        cout << "child process abortion" << endl;
                        exit(0);
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
    int len = strlen(command);
    for(int i = 0; i < len; i++){       // format input commands like "command;command" as "command ; command"
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
        //cout << "execution!" << endl;
    }

}

int main(){
    get_command();
    return 0;
}
