#include <pwd.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iomanip>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>

using namespace std;

//for prompt
void type_prompt();

int isTarget(char *path, char *target);

char * isPath(char * cmd);

void cmdToArgv(char * temp, char ** argvTemp);

//for |
void pipe(char ** argvLeftSide, char ** argvRightSide);

//for >
void outRedirection(char ** argvLeftSide, char ** argvRightSide);

//for <
void inRedirection(char ** argvLeftSide, char ** argvRightSide);

//for <<<
void triInRedirection(char ** argvLeftSide, char ** argvRightSide);

//for 2>
void twoOutRedirection(char ** argvLeftSide, char ** argvRightSide);

void cmdExec(char **argv);

int execv(char ** argv);

void pipeExec(char **argv);

bool redirectionJudge(char ** argv);

bool connectorJudge(char **argv);

void freeArgv(char ** temp);



