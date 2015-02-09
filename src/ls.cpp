#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
char pathab[1000];
char t;

using namespace std;

#define MAXROWLEN   80
#define NAME_MAX    255
#define MYPATH_MAX    1024
#define PARAM_NONE  0 //no param
#define PARAM_A     1 //-a
#define PARAM_L     2 //-l
#define PARAM_R     4 //-R

int g_leave_len = MAXROWLEN;
int g_maxlen;

void my_error(const char* errstring, int line) {
    fprintf(stderr,"line:%d  ",line);
    perror(errstring);
    exit(1);
}

void display_single(char *name) {
    int i,len;

    if(g_leave_len < g_maxlen) {
        printf("\n");
        g_leave_len = MAXROWLEN;
    }
    
    len = strlen(name);
    len = g_maxlen - len;
    if(t == 'd')
        printf("\033[1;40;32m%s\033[0m",name);
    else if(name[strlen(name)-1]=='p' && name[strlen(name)-2]=='p' 
        && name[strlen(name)-3]=='c' && name[strlen(name)-4]=='.')
                printf("\033[1;40;31m%s\033[0m",name);
    else
        printf("%-s\t",name);
    
    for(i=0;i<len;i++)
        printf(" ");
    printf(" ");
    
    g_leave_len = g_leave_len - g_maxlen - 2;
}

void display_attribute(struct stat buf, char *name) {
    char buf_time[32];
    struct passwd *psd;
    struct group *grp;
  
    //show the property of the file
    cout << t;
    //show the privilidge of the file
    cout << (buf.st_mode & S_IRUSR? "r" : "-");
    cout << (buf.st_mode & S_IWUSR? "w" : "-");
    cout << (buf.st_mode & S_IXUSR? "x" : "-");
    
    cout << (buf.st_mode & S_IRGRP? "r" : "-");
    cout << (buf.st_mode & S_IWGRP? "w" : "-");
    cout << (buf.st_mode & S_IXGRP? "x" : "-");
    
    cout << (buf.st_mode & S_IROTH? "r" : "-");
    cout << (buf.st_mode & S_IWOTH? "w" : "-");
    cout << (buf.st_mode & S_IXOTH? "x " : "- ");
  
    psd = getpwuid(buf.st_uid);
    grp = getgrgid(buf.st_gid);
    printf("%4d  ",buf.st_nlink);   // print the file it contains
    printf("%-8s ",psd->pw_name);   // print the owner
    printf("%-8s ",grp->gr_name);   // print the group
    
    printf("%8lld",buf.st_size);      // print the file size
    strcpy(buf_time, ctime(&buf.st_mtime)); // turn the seconds to the dates
    buf_time[strlen(buf_time) - 1] = 0;
    printf(" %s ",buf_time);
}

void display(int flag,char *pathname, char *name) {
    int i,j;
    struct stat buf;
//    char name[NAME_MAX + 1];

    if(lstat(pathname,&buf) == -1) {
        my_error("stat",__LINE__);
    }

    t = (S_ISLNK(buf.st_mode)?'1':S_ISREG(buf.st_mode)?'-':
             S_ISDIR(buf.st_mode)?'d':S_ISCHR(buf.st_mode)?'c':
             S_ISBLK(buf.st_mode)?'b':S_ISFIFO(buf.st_mode)?'f':
             S_ISSOCK(buf.st_mode)?'s':' ');

    if(flag == PARAM_NONE) {
        if(name[0] != '.')
            display_single(name);
    }
    
    else if(flag == PARAM_A)
        display_single(name);
    
    else if(flag == PARAM_L) {
        if(name[0] != '.') {
            display_attribute(buf,name);
            if(t=='d')
                printf("\033[1;40;32m%s\033[0m\n",name);
            else if(name[strlen(name)-1]=='p' && name[strlen(name)-2]=='p' 
                && name[strlen(name)-3]=='c' && name[strlen(name)-4]=='.')
                printf("\033[1;40;31m%s\033[0m\n",name);
            else
            cout << name << endl;
        }
    }

    else if(flag == (PARAM_A | PARAM_L)) {
        display_attribute(buf,name);
        if(t=='d')
            printf("\033[1;40;32m%s\033[0m\n",name);
        else if(name[strlen(name)-1]=='p' && name[strlen(name)-2]=='p' 
                && name[strlen(name)-3]=='c' && name[strlen(name)-4]=='.')
                printf("\033[1;40;31m%s\033[0m\n",name);
        else
            cout << name << endl;
            
    }
}

void display_dir(int flag_param,const char *path) {
    DIR* dir;
    struct dirent* dirent;
    char filenames[256][MYPATH_MAX+1],temp[MYPATH_MAX+1];
    char pathnames[256][MYPATH_MAX+1];
    int count = 0;
    
    if((dir = opendir(path)) == NULL) {
        my_error("opendir",__LINE__);
    }
    
    while((dirent = readdir(dir)) != NULL) {
        if(g_maxlen < strlen(dirent->d_name))
            g_maxlen = strlen(dirent->d_name);
        count++;
    }
    if(closedir(dir)==-1)
        my_error("closedir error",__LINE__);
    
    if(count>256)
        my_error("too more input file(more than 256)",__LINE__);
    
    int i,j,len = strlen(path);
    
    if((dir = opendir(path)) == NULL) {
        my_error("opendir",__LINE__);
    }
    for(i=0;i<count;i++) {
        dirent = readdir(dir);
        if(dirent == NULL) {
            my_error("readdir",__LINE__);
        }
        strcpy(pathnames[i],path);
        char temp[5];
        strcpy(temp,"/");
        strcat(pathnames[i],temp);
        strcat(pathnames[i],dirent->d_name);
        strcpy(filenames[i],dirent->d_name);

        pathnames[i][len+strlen(dirent->d_name)+1] = 0;
    }
    
    for(i=0;i<count-1;i++)
        for(j=i+1;j<count-1;j++) {
            if(strcmp(filenames[i],filenames[j]) > 0) {
                strcpy(temp,filenames[j]);
                strcpy(filenames[j] , filenames[i]);
                strcpy(filenames[i] , temp);
            }
        }

    for(i=0;i<count;i++)
        if(flag_param>=4) 
            display(flag_param-4,pathnames[i],filenames[i]);
        else
            display(flag_param,pathnames[i],filenames[i]);

    if(closedir(dir)==-1)
        my_error("closedir error",__LINE__);

    if((flag_param & PARAM_L) == 0)
        printf("\n");
    
    if(flag_param >= 4) {
        for(i=0;i<count;i++) {
            struct stat dirbuf;
            char* temp = pathnames[i];

            if(temp[strlen(temp)-1]=='.') continue; // skip ./. and ./..
            
            if(lstat(temp,&dirbuf) == -1) {
                my_error("stat ",__LINE__);
            }
            else if(S_ISDIR(dirbuf.st_mode)) {
               printf("\033[1;40;32m%s\033[0m:\n",filenames[i]);
                display_dir(flag_param,temp);
            }
        }
    }
}

int main(int argc, char **argv) {
    int i,j,k;
    int num; 
    char path[MYPATH_MAX + 1];
    char param[1000];
    int  flag_param = PARAM_NONE;
    struct stat buf;
    
    j = 0;
    num = 0;
    for(i=1;i<argc;i++) {
        if(argv[i][0] == '-') {
            for(k=1;k<strlen(argv[i]);k++) {
                if(argv[i][k] == 'a' || argv[i][k] == 'l' || argv[i][k] == 'R') {
                    param[j] = argv[i][k];
                    j++;
                }
                else{
                    cout << "error parameter：" << argv[i][k] << endl;
                    exit(1);
                }

            }
            num++;
        }
        else{
            cout << "error parameter：" << argv[i] << endl;
            exit(1);
        }
    }
    
    // input argv      param[]
    // -a -l           al
    // -al             al
    
    for(i=0;i<j;i++) {
        if(param[i] == 'a') {
            flag_param |= PARAM_A;  // flag_param = **1
        }
        else if(param[i] == 'l') {
            flag_param |= PARAM_L;  // flag_param = *1*
        }
        else if(param[i] == 'R') {
            flag_param |= PARAM_R;  // flag_param = 1**
        }
        else {
            cout << "error parameter：" << param[i] << endl;
            exit(1);
        }
    }
    
    param[j] = 0;
    getcwd(pathab,1000);
    strcpy(path,pathab);
    display_dir(flag_param,path);
    return 0;
}
