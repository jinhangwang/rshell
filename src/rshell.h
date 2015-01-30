#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>

using namespace std;

const int max_name_len = 256;
const int max_path_len = 1024;

void type_prompt(){
    
    struct passwd *pwd;
    char hostname[max_name_len];
    char pathname[max_path_len];
    
    pwd = getpwuid(getuid());
    getcwd(pathname,max_path_len);
    gethostname(hostname,max_name_len);
    cout << "[rshell]" << pwd->pw_name << "@" << hostname << "$:";
}
