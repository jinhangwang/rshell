#include "rshell.h"

using namespace std;

char* get_command(){
    type_prompt();
    char *command = new char[100];
    char ccmd;
    memset(command, '\0', 100);
    cin.getline(command, 10000);      // get the command
    strncat(command, "\0", 1);
    return command;
}

char* prehandling_command(char* command){    
    int len = (unsigned int)strlen(command);
    char *afterCommand = new char[100];
    memset(afterCommand, '\0',100);
    for(int i = 0, j = 0; i < len; i++, j++){       // format input commands like "command_a;command_b" as "command_a ; command_b"
        if(command[i] == '#'){
            afterCommand[j] = '\0';
            break;
        }
        else if(command[i] == ';'){
            afterCommand[j] = ' '; afterCommand[j+1] = ';'; afterCommand[j+2] = ' ';
            j+=2;
        }
        else if(command[i] == '&' && command[i+1] == '&'){
            afterCommand[j] = ' '; afterCommand[j+1] = '&'; afterCommand[j+2] = '&'; afterCommand[j+3] = ' ';
            j+=3;i++;
        }
        else if(command[i] == '|' && command[i+1] == '|'){
            afterCommand[j] = ' '; afterCommand[j+1] = '|'; afterCommand[j+2] = '|'; afterCommand[j+3] = ' ';
            j+=3;i++;
        }
        else if(command[i] == '>' && command[i+1]!='>'){
            afterCommand[j] = ' '; afterCommand[j+1] = '>'; afterCommand[j+2] = ' ';
            j+=2;
        }
        else if(command[i] == '<' && command[i+1] != '<'){
            afterCommand[j] = ' '; afterCommand[j+1] = '<'; afterCommand[j+2] = ' ';
            j+=2;
        }        
        else if(command[i] == '<' && command[i+1] == '<' && command[i+2] == '<'){
            afterCommand[j] = ' '; afterCommand[j+1] = '<'; afterCommand[j+2] = '<';
            afterCommand[j+3] = '<'; afterCommand[j+4] = ' ';
            j+=4;i+=2;
        }        
        else if(command[i] == '|' && command[i+1]!='|'){
            afterCommand[j] = ' '; afterCommand[j+1] = '|'; afterCommand[j+2] = ' ';
            j+=2;
        }
        else if(command[i] == '>' && command[i+1] == '>'){
            afterCommand[j] = ' '; afterCommand[j+1] = '>'; afterCommand[j+2] = '>'; afterCommand[j+3] = ' ';
            j+=3;i++;
        }
        else if(command[i] == '2' && command[i+1] == '>'){
            afterCommand[j] = ' '; afterCommand[j+1] = '2'; afterCommand[j+2] = '>'; afterCommand[j+3] = ' ';
            j+=3;i++;
        }    
        else if(command[i] == '\t');
        else
            afterCommand[j] = command[i];
    }
    afterCommand[strlen(afterCommand)]='\0';
    char* cmd = new char[100];
    memset(cmd, '\0',100);
    int i = 0;
    while(afterCommand[i] != '\0'){
        if(afterCommand[i] == ' '){
            while(afterCommand[i+1] == ' '){
                if(afterCommand[i+1] == '\0')
                    break;
                else
                    i++;
            }
        }
        strncat(cmd, &afterCommand[i],1);
        i++;
    }
    cmd[strlen(cmd)]='\0';
    return cmd;
}

void loop(){
    int pid = fork();
    int childpid = 1;
    if(pid == 0){    
        while(true){
            childpid = getpid();
            char * cmd;
            char * argvRightSide[50];
            cmd = get_command();
            cmd = prehandling_command(cmd);
            cmdToArgv(cmd, argvRightSide);

            if(strcmp(argvRightSide[0],"exit") == 0){
                freeArgv(argvRightSide);
                if(-1 == kill(getppid(),SIGKILL))
                    perror("kill");       
            }
            cmdExec(argvRightSide);
        }
        exit(0);
    }
    else if(pid ==-1)
        perror("main fork fail");
    else{
        wait(0);
        loop();
    }
}

int main(){
    loop();
    return 0;
}


