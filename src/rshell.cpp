#include "rshell.h"

const int max_name_len = 256;
const int max_path_len = 1024;
int MAX_LEN = 10000;
static bool stop = false;

void type_prompt(){
    
    struct passwd *pwd;
    char hostname[max_name_len];
    char pathname[max_path_len];
    
    if(NULL==(pwd = getpwuid(getuid())))
    	perror("getpwuid err");
    if(NULL==(getcwd(pathname,max_path_len)))
    	perror("getcwd err");
    if(-1==(gethostname(hostname,max_name_len)))
    	perror("gethostname err");
    cout << "[rshell]" << pwd->pw_name << "@" << hostname << "$:";
}

int isTarget(char *path, char *target){
    DIR *dirp = opendir(path);
    if(dirp == NULL){
        return 0;
        perror("some directories cannot open without permmission");
    }
    
    dirent *direntp;
    while((direntp = readdir(dirp)))
        if(strcmp(direntp->d_name,target) == 0){
            if(-1 == closedir(dirp))
                perror("closedir");
            return 1;
        }
    if(-1 == closedir(dirp))
        perror("closedir");
    return 0;
}

char* isPath(char * cmd){
    char * path[50];
    char * tempPath = getenv("PATH");
    
    int i = 0;
    char *tok;
    tok = strtok(tempPath, ":");
    for(; tok != NULL; i++){
        path[i] = new char[strlen(tok) + 1];
        memset(path[i], '\0',strlen(path[i]));
        strcpy(path[i], tok);
        strncat(path[i], "\0", 1);
        tok = strtok(NULL, ":");
    }
    for(; i != 50; i++)
        path[i] = NULL;
    
    for(int j = 0;path[j] != NULL; j++)
        if(isTarget(path[j], cmd) == 1){       
            strncat(path[j],"/",1);
            strncat(path[j],"\0",1);
            return path[j];
        }   
    return NULL;
}

void cmdToArgv(char * temp, char ** argvTemp){
    int i = 0;
    char *tok;
    tok = strtok(temp, " ");
    while(tok != NULL && (strcmp(tok, "#") != 0)){
        argvTemp[i] = new char[strlen(tok) + 1];
        memset(argvTemp[i], '\0',strlen(argvTemp[i]));
        strcpy(argvTemp[i], tok);
        strncat(argvTemp[i], "\0", 1);
        i++;
        tok = strtok(NULL, " ");
    }
    for(; i != 50; i++)
        argvTemp[i] = NULL;
    delete [] temp;
    temp = NULL;
}

// pipe |
void pipe(char ** argvLeftSide, char ** argvRightSide){
    int fd[2];
    if(-1 == pipe(fd))
        perror("pipe() err ");
    int pid = fork();
    if(-1 == pid)
        perror("fork() err ");
    // ----------- parent thread -----------
    else if(pid > 0){
        int stdin_bak;
        if(-1 == (stdin_bak = dup(0)))
            perror("dup err");
        if(-1 == dup2(fd[0],0))//make stdin the read end of the pipe
            perror("dup2 err");
        if(-1 == close(fd[1]))
            perror("close err");
        if( -1 == wait(0))
            perror("wait() err");
        int fpid = fork();
        if(fpid == 0){
            char * commandPath = isPath(argvRightSide[0]);
            if(commandPath == NULL){
                cerr << "not a command" << endl;
                exit(1);
            }
            else{
                strcat(commandPath,argvRightSide[0]);
                strncat(commandPath,"\0",1);
                if(execv(commandPath, argvRightSide) == -1){
                    perror(argvRightSide[0]);
                    exit(1);
                }
            }
            exit(0);
        }
        else if(fpid == -1)
            perror("fork fail");
        else{
            if(-1 == wait(NULL))
                perror("wait err");
        }
        if(-1 == dup2(stdin_bak,0))//restore stdin
            perror("dup2 err");
    }
    // ----------- child thread -----------
    else if(pid == 0){
        if(-1 == dup2(fd[1],1))
            perror("dup2 err");
        if(-1 == close(fd[0]))
            perror("close err");
        if(redirectionJudge(argvLeftSide))
            pipeExec(argvLeftSide);
        else{
            char * commandPath = isPath(argvLeftSide[0]);
            if(commandPath == NULL){
                cerr << "not a command" << endl;
                exit(1);
            }
            else{
                strcat(commandPath,argvLeftSide[0]);
                strncat(commandPath,"\0",1);
                //const char * cmd = commandPath;
                if(execv(commandPath, argvLeftSide) == -1){
                    perror(argvLeftSide[0]);
                    exit(1);
                }
            }
        }
        exit(0);
    }
}

// pipe >
void outRedirection(char **argvLeftSide, char ** argvRightSide){
    int fd[2];
    if(-1 == pipe(fd))
        perror("pipe() err");
    
    int filePID;
    filePID = open(argvRightSide[0], O_RDWR | O_CREAT,S_IRWXU);
    if(filePID == -1)
        perror("file open err");
    int pid = fork();
    if(-1 == pid){
        perror("fork err");
        exit(1);
    }
    // ----------- parent thread -----------
    else if(pid > 0) //parent function
    {
        int stdin_bak;
        if(-1 == (stdin_bak = dup(0)))
            perror("dup err");
        if(-1 == dup2(fd[0],0))//make stdin the read end of the pipe
            perror("dup2 err ");
        if(-1 == close(fd[1]))
            perror("close err ");
        if( -1 == wait(0))
            perror("wait() err ");
        
        char buf[BUFSIZ];
        memset(buf, '\0', BUFSIZ);
        if(-1 == read(fd[0],buf,BUFSIZ))
            perror("read");
        if(-1 == write(filePID, buf, BUFSIZ))
            perror("write");
        if(-1 == dup2(stdin_bak,0))
            perror("dup2 err");
    }
    // ----------- child thread -----------
    else if(pid == 0){
        if(-1 == dup2(fd[1],1))//make filePID the write end of the pipe
            perror("dup2 err");
        if(-1 == close(fd[0]))
            perror("close err");
        if(redirectionJudge(argvLeftSide))
            pipeExec(argvLeftSide);
        else{
            char *commandPath;
            commandPath = isPath(argvLeftSide[0]);
            if(commandPath == NULL){
                cerr << argvLeftSide[0] << "not a command" << endl;
                exit(1);
            }
            else{
                strcat(commandPath, argvLeftSide[0]);
                strncat(commandPath, "\0", 1);
                if(-1 == execv(commandPath, argvLeftSide)) {
                    perror("execv err ");
                    exit(1);
                }
            }
        }
        exit(0);
    }
    if(-1 == close(filePID))
        perror("filePID close err");
}

// pipe <
void inRedirection(char ** argvLeftSide, char ** argvRightSide){
    int fd[2];
    if(-1 == pipe(fd))
        perror("pipe() err ");
    
    int filePID;
    filePID = open(argvRightSide[0], O_RDWR | O_CREAT, S_IRWXU);
    if(filePID == -1)
        perror("open");
    int pid = fork();
    if(-1 == pid){
        perror("fork() err ");
        exit(1);
    }
    // ----------- parent thread -----------
    else if(pid > 0) //parent function
    {
        int stdin_bak;
        if(-1 == (stdin_bak = dup(0)))
            perror("dup err");
        if(-1 == dup2(fd[0],0))//make stdin the read end of the pipe
            perror("dup2 err ");
        if(-1 == close(fd[1]))
            perror("close err ");
        if( -1 == wait(0))
            perror("wait() err ");
        if(redirectionJudge(argvLeftSide))
            pipeExec(argvLeftSide);
        else{
            int fpid = fork();
            if(fpid == 0){
                char * commandPath = isPath(argvLeftSide[0]);
                if(commandPath == NULL){
                    cerr << argvLeftSide[0] <<"not a command" << endl;
                    exit(1);
                }
                else{
                    strcat(commandPath, argvLeftSide[0]);
                    strncat(commandPath,"\0",1);
                    if(execv(commandPath, argvLeftSide) == -1){
                        perror("execv err");
                        exit(1);
                    }
                }
                exit(0);
            }
            else if(fpid == -1)
                perror("fork fail");
            else{
                if(-1 == wait(NULL))
                    perror("wait");
            }
        }
        if(-1 == dup2(stdin_bak,0))
            perror("dup2 err");
    }
    // ----------- child thread -----------
    else if(pid == 0){
        //write to the pipe
        if(-1 == dup2(fd[1],1))//make filePID the write end of the pipe
            perror("dup2 err ");
        if(-1 == close(fd[0]))
            perror("close err ");
        
        char buf[BUFSIZ];
        memset(buf, '\0', BUFSIZ);
        if(-1 == read(filePID,buf,BUFSIZ))
            perror("read");
        if(-1 == write(fd[1], buf, BUFSIZ))
            perror("write");
        exit(0);
    }
}

// pipe <<<
void triInRedirection(char ** argvLeftSide, char ** argvRightSide){
    int fd[2];
    
    if(-1 == pipe(fd))
        perror("pipe() err ");
    int pid = fork();
    if(-1 == pid)
        perror("fork() err ");
    // ----------- parent thread -----------
    else if(pid > 0) //parent function
    {
        int stdin_bak;
        if(-1 == (stdin_bak = dup(0)))
            perror("call dup fail");
        if(-1 == dup2(fd[0],0))//make stdin the read end of the pipe
            perror("dup2 err ");
        if(-1 == close(fd[1]))
            perror("close err ");
        if( -1 == wait(0))
            perror("wait() err ");
        if(redirectionJudge(argvLeftSide))
            pipeExec(argvLeftSide);
        else{
            int fpid = fork();
            if(fpid == 0){
                char * commandPath = isPath(argvLeftSide[0]);
                if(commandPath == NULL){
                    cerr << argvLeftSide[0] << "not a command"<< endl;
                    exit(1);
                }
                else{
                    strcat(commandPath, argvLeftSide[0]);
                    strncat(commandPath, "\0",1);
                    if(execv(commandPath, argvLeftSide) == -1){
                        perror("execv err");
                        exit(1);
                    }
                }
                exit(0);
            }
            else if(fpid == -1)
                perror("fork fail");
            else{
                if(-1 == wait(NULL))
                    perror("wait");
            }
        }
        if(-1 == dup2(stdin_bak,0))
            perror("call dup fail");
    }
    // ----------- child thread -----------
    else if(pid == 0){
        if(-1 == dup2(fd[1],1))  //write stdout
            perror("dup2 err ");
        if(-1 == close(fd[0]))
            perror("close err ");
        
        int i = 0;
        int size = 0;
        for(;argvRightSide[i] != NULL;i++)
            size += strlen(argvRightSide[i]) + 1;
        size+=2;
        char *buf = new char[size];
        memset(buf, '\0', size);
        for(i = 0; argvRightSide[i] != NULL; i++){
            strcat(buf, argvRightSide[i]);
            strncat(buf, " ",1);
        }
        strncat(buf, "\n", 1);
        strncat(buf, "\0", 1);
        if(-1 == write(fd[1], buf, size))
            perror("write");
        delete [] buf;
        buf = NULL;
        exit(0);
    }
}

// pipe 2>
void twoOutRedirection(char ** argvLeftSide, char ** argvRightSide){
    int fd[2];
    if(-1 == pipe(fd))
        perror("pipe() err ");
    
    int filePID;
    filePID = open(argvRightSide[0], O_RDWR | O_CREAT, S_IRWXU);
    if(filePID == -1)   
        perror("open");
    int pid = fork();
    if(-1 == pid)
        perror("fork() err ");
    // ----------- parent thread -----------
    else if(pid > 0){
        int stdin_bak;
        if(-1 == (stdin_bak = dup(0)))
            perror("dup err");
        if(-1 == dup2(fd[0],0))
            perror("dup2 err ");
        if(-1 == close(fd[1]))
            perror("close err ");
        if( -1 == wait(0)) 
            perror("wait() err ");
        
        char buf[BUFSIZ];
        memset(buf, '\0', BUFSIZ);
        if(-1 == read(fd[0],buf,BUFSIZ))
            perror("read");
        if(-1 == write(filePID, buf, BUFSIZ))
            perror("write");
        if(-1 == dup2(stdin_bak,0))
            perror("dup2 err");
    }
    // ----------- child thread -----------
    else if(pid == 0){
        if(-1 == dup2(fd[1],2))//make filePID the write end of the pipe 
            perror("dup2 err ");
        if(-1 == close(fd[0]))
            perror("close err ");
        if(redirectionJudge(argvLeftSide))
            pipeExec(argvLeftSide);
        else {
            char * commandPath = isPath(argvLeftSide[0]);
            if(commandPath == NULL){
                cerr << argvLeftSide[0] << "not a command"<< endl;
                exit(1);
            }
            else{
                strcat(commandPath, argvLeftSide[0]);
                strncat(commandPath, "\0",1);
                if(execv(commandPath, argvLeftSide) == -1){ 
                    delete [] commandPath;
                    commandPath = NULL;
                    perror("execv err");
                    exit(1);
                }
            }
        }
        exit(0);  
    }
    if(-1 == close(filePID))
        perror("filePID close err");
}

void cmdExec(char **argv){
    if(connectorJudge(argv)){
        int i = 0, j = 0, wait_pid;
        char *subArgv[20];
        while(argv[i] != NULL){
            if(strcmp(argv[i],";") == 0 || strcmp(argv[i],"&&") == 0 || strcmp(argv[i],"||") == 0){
                subArgv[j] = NULL;
                wait_pid = execv(subArgv);
                freeArgv(subArgv);
                if((strcmp(argv[i],";") == 0 || strcmp(argv[i], "&&") == 0) && wait_pid != 0){
                    freeArgv(subArgv);
                    stop = true;
                    break;
                }
                //store left of commands
                if(argv[i+1] != NULL){
                    int tempIndex = i + 1, index = 0;
                    char *leftArgv[50];
                    while(argv[tempIndex] != NULL){
                        leftArgv[index] = new char[strlen(argv[tempIndex])+ 1];
                        memset(leftArgv[index],'\0',strlen(leftArgv[index]));
                        strcpy(leftArgv[index], argv[tempIndex]);
                        strncat(leftArgv[index],"\0", 1);
                        index++;
                        tempIndex++;
                    }
                    leftArgv[index] = NULL;
                    cmdExec(leftArgv);
                    freeArgv(leftArgv);
                    if(stop) break;
                }
                else break;
            }
            subArgv[j] = new char[strlen(argv[i]) + 1];
            memset(subArgv[j],'\0',strlen(subArgv[j]));
            strcpy(subArgv[j], argv[i]);
            strncat(subArgv[j], "\0", 1);
            j++; i++;
        }
    }
    else{
        stop = true;
        execv(argv);
    }
}

int execv(char ** argv){
    int wait_pid = 0;
    if(redirectionJudge(argv)){
        pipeExec(argv);
        return wait_pid;
    }
    else{
        if(strcmp(argv[0], "exit") == 0){
            freeArgv(argv);
            exit(0);
        }
        int pid = fork();
        if(pid == 0){
            char * commandPath = isPath(argv[0]);
            if(commandPath == NULL){
                cerr << "there is no such command" << endl;
                exit(1);
            }
            else{
                strcat(commandPath,argv[0]);
                strncat(commandPath,"\0",1);
                if(execv(commandPath, argv) == -1){
                    perror(argv[0]);
                    exit(1);
                }
            }
        }
        else if(-1 == pid){
            perror("fork");
            freeArgv(argv);
            exit(1);
        }
        else{
            if(waitpid(pid,&wait_pid,0) == -1){
                perror("wait");
                exit(1);
            }
        }
        return wait_pid;
    }
}

void pipeExec(char **argv){
    char *argvLeftSide[50];
    char *argvRightSide[50];
    char *redirectConnector = new char[10];
    memset(redirectConnector,'\0',10);
    
    //>>>>>>>>>>>>>>
    int i = 0, j = 0, index = 0;
    while(argv[i] != NULL)
        i++;
    for(i-=1; i >= 0; i--){
        if(strcmp(argv[i],"<") == 0 ||
           strcmp(argv[i],"<<<") == 0 || strcmp(argv[i],"2>") == 0 ||
           strcmp(argv[i],">") == 0 || strcmp(argv[i],"|") == 0 ) {
            strcpy(redirectConnector, argv[i]);
            j = i; i++;
            while(argv[i] != NULL){
                argvRightSide[index] = new char[strlen(argv[i])];
                memset(argvRightSide[index],'\0',strlen(argvRightSide[index]));
                strcpy(argvRightSide[index],argv[i]);
                index++;
                i++;
            }
            argvRightSide[index] = NULL;
            break;
        }
    }
    for(i=0; i != j; i++){
        argvLeftSide[i] = new char[strlen(argv[i])];
        memset(argvLeftSide[i],'\0',strlen(argvLeftSide[i]));
        strcpy(argvLeftSide[i],argv[i]);
    }
    argvLeftSide[i] = NULL;

    if(strcmp(redirectConnector,"|") == 0)
        pipe(argvLeftSide,argvRightSide);
    else if(strcmp(redirectConnector,">") == 0)
        outRedirection(argvLeftSide,argvRightSide);
    else if(strcmp(redirectConnector,"<") == 0)
        inRedirection(argvLeftSide,argvRightSide);
    else if(strcmp(redirectConnector,"<<<") == 0)
        triInRedirection(argvLeftSide,argvRightSide);
    else if(strcmp(redirectConnector,"2>") == 0)
        twoOutRedirection(argvLeftSide,argvRightSide);

    delete [] redirectConnector;
    redirectConnector = NULL;    
}

bool redirectionJudge(char ** argv){
    for(int i = 0; argv[i] != NULL; i++)
        if(strcmp(argv[i],"<") == 0 || strcmp(argv[i],">") == 0 ||
           strcmp(argv[i],"|") == 0 || strcmp(argv[i],"<<<") == 0 ||
           strcmp(argv[i],"2>") == 0)
            return true;
    return false;
}

bool connectorJudge(char **argv){
    for(int i = 0; argv[i] != NULL; i++)
        if(strcmp(argv[i], ";") == 0 || strcmp(argv[i],"&&") == 0 || strcmp(argv[i],"||") == 0)
            return true;
    return false;
}

void freeArgv(char ** temp){
    for(int i = 0; temp[i] != NULL; i++){
        delete [] temp[i]; temp[i] = NULL;
    }
}