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

using namespace std;

#define PARAM_NONE  0 //no param
#define PARAM_A     1 //-a
#define PARAM_L     2 //-l
#define PARAM_R     4 //-R
#define MAXROWLEN   80
#define NAME_MAX    255
#define PATH_MAX    1024

int g_leave_len = MAXROWLEN;
int g_maxlen;

void my_error(const char* errstring, int line) {
    fprintf(stderr,"line:%d",line);
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
    
    printf("%-s",name);
    
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
    cout << (S_ISLNK(buf.st_mode)?"1":S_ISREG(buf.st_mode)?"-":
             S_ISDIR(buf.st_mode)?"d":S_ISCHR(buf.st_mode)?"c":
             S_ISBLK(buf.st_mode)?"b":S_ISFIFO(buf.st_mode)?"f":
             S_ISSOCK(buf.st_mode)?"s":"");

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

void display(int flag,char *pathname) {
    int i,j;
    struct stat buf;
    char name[NAME_MAX + 1];

    
    for(i=0,j=0;i<strlen(pathname);i++) {
        if(pathname[i] == '/')
            j = 0;
        else
            name[j++] = pathname[i];
    }
    name[j] = 0;
    
    if(lstat(pathname,&buf) == -1) {
        my_error("stat",__LINE__);
    }
    
    if(flag == PARAM_NONE) {
        if(name[0] != '.')
            display_single(name);
    }
    
    else if(flag == PARAM_A)
        display_single(name);
    
    else if(flag == PARAM_L) {
        if(name[0] != '.') {
            display_attribute(buf,name);
            cout << name << endl;
        }
    }

    else if(flag == (PARAM_A | PARAM_L)) {
        display_attribute(buf,name);
        cout << name << endl;
    }

/*    
    else if(flag == PARAM_R) {
        if(name[0] != '.') {
            display_attribute(buf,name);
            cout << name << endl;
            if(S_ISDIR(buf.st_mode))

        }
    }
*/

/*
    else if(flag == (PARAM_A | PARAM_R)) {
        cout << "do -aR!" << endl;
        //display_attribute(buf,name);
        //cout << name << endl;
    }

    else if(flag == (PARAM_L | PARAM_R)) {
        cout << "do -lR!" << endl;
        //display_attribute(buf,name);
        //cout << name << endl;
    }
    else if(flag == (PARAM_A | PARAM_L | PARAM_R)) {
        cout << "do -alR!" << endl;
        //display_attribute(buf,name);
        //cout << name << endl;
    }
*/
}

void display_dir(int flag_param,const char *path) {
    DIR* dir;
    struct dirent* dirent;
    char filenames[256][PATH_MAX+1],temp[PATH_MAX+1];
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
        strncpy(filenames[i],path,len);
        filenames[i][len] = 0;
        strcat(filenames[i],dirent->d_name);
        filenames[i][len+strlen(dirent->d_name)] = 0;
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
            display(flag_param-4,filenames[i]);
        else
            display(flag_param,filenames[i]);

    if(closedir(dir)==-1)
        my_error("closedir error",__LINE__);

    if((flag_param & PARAM_L) == 0)
        printf("\n");
    
    if(flag_param >= 4) {
        for(i=0;i<count;i++) {
            struct stat dirbuf;
            strcpy(temp,filenames[i]);
            if(lstat(temp,&dirbuf) == -1) {
                my_error("stat ",__LINE__);
            }
            else if(S_ISDIR(dirbuf.st_mode)) {
                cout << filenames[i] << endl;
                for (j = 0; j < strlen(temp)-2; ++j)
                {
                     temp[j] = temp[j+2];   
                }
                display_dir(flag_param,temp);
            }
        }
    }
}

int main(int argc, char **argv) {
    int i,j,k;
    int num; 
    char path[PATH_MAX + 1];
    char param[1000];
    int  flag_param = PARAM_NONE;
    struct stat buf;
    
    j = 0;
    num = 0;
    for(i=1;i<argc;i++) {
        if(argv[i][0] == '-') {
            for(k=1;k<strlen(argv[i]);k++) {
                param[j] = argv[i][k];
                j++;
            }
            num++;
        }
    }
    
    // input argv      param[]
    // -a -l           al
    // -al             al

//    cout << "param[] = " << param << endl;
    
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
//    cout << "flag_param = " << flag_param << endl;
    
    param[j] = 0;

    strcpy(path,"./");
    path[2] = 0;
    display_dir(flag_param,path);
    return 0;
}
