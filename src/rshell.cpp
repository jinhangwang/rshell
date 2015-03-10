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
