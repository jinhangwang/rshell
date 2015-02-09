#include <iostream>
#include <fstream>
#include <stdio.h>
#include <time.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#define BUFSIZE 1024

using namespace std;

void my_cpfile(const char *spath,const char *dpath,const int time_flag){
    DIR *sdir;
    struct dirent *ddir;
    char sname[1000];   //std::istream::get and std::ostream::put
    char ch;            //read and write
    char buffer[BUFSIZE];
    char chbuff[1];
    long read_buff;
    long write_buff;

    clock_t start, finish;
    double timespan;
    
    char temp[1000];
    char temp_path[1000];
    struct stat s;
    char *p = strrchr((char*)spath, '/');
    if(stat(spath,&s)==-1){
        perror("err in stat");
        return;
    }
    p++;
    strcpy(sname,(char*)p);
    
    if(NULL==(sdir=opendir(dpath))){
        perror("err in opendir");
        return;
    }
    while ((ddir=readdir(sdir))!=NULL){
        if(ddir->d_name[0]=='.')
            continue;
        if (0 == strcmp(ddir->d_name,sname)){
            cout<<"ERROR: the file is already existed"<<endl;
            exit(0);
        }
    }

    strcpy(temp,(char*)"/");
    strcat(temp,sname);
    strcpy(temp_path,dpath);
    strcat(temp_path,temp);
    
    ofstream outfile (temp_path,ios::binary);
    ifstream infile (spath,ios::binary);
    
    //read and write
    int sfp;
    if (-1==(sfp=open(spath,O_RDONLY))){
        perror("err in open src file");
    }
    int dstfp;
    if (-1==(dstfp=open(temp_path,O_WRONLY|O_CREAT))){
        perror("err in open dst file");
    }
    
    if (time_flag == 0){   
        // read and write with buff
        if(sfp){
            while(BUFSIZE-1<(read_buff=(read(sfp,buffer,BUFSIZE)))){
                if (read_buff == -1){
                    perror("err in read");
                    break;
                }
                else if (read_buff > 0){
                    while (-1!=(write_buff=(write(dstfp,buffer,BUFSIZE)))){
                        if(write_buff == -1){
                            perror("err in write");
                        }
                        if(read_buff == write_buff)
                            break;
                    }
                }
            }
            write_buff=write(dstfp,buffer,read_buff);
            if(write_buff == -1){
                perror("err in write");
            }
        }
    }
    else if (time_flag == 1 ){
        //time
        struct stat buf;
        if(lstat(dpath,&buf) == -1){
            perror("err in stat");
        }
        char buf_time[40];
        strcpy(buf_time, ctime(&buf.st_mtime));
        printf("%s",buf_time);
        
        // show the user name
        struct passwd *pwd=NULL;
        if (getpwuid(getuid()) == NULL)
            perror("err in getpwuid");
        else
            pwd = getpwuid(getuid());
        cout<<"User:  " << pwd->pw_name << endl;
        
        // time of std::istream::get and std::ostream::put
        if (-1==(sfp=open(spath,O_RDONLY)))
            perror("err in open src file");
        if (-1==(dstfp=open(temp_path,O_WRONLY|O_CREAT)))
            perror("err in open dst file");

        start = clock();
        while(infile.get(ch))
            outfile.put(ch);
        finish = clock();
        
        timespan = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << timespan * 1000 << "ms with std::istream::get and std::istream::put" << endl;
        remove(temp_path);

        // time of read and write with buff
        start = clock();
        if(sfp)
        {
            while(BUFSIZE-1<(read_buff=(read(sfp,buffer,BUFSIZE)))){
                if (read_buff == -1){
                    perror("err in read");
                    break;
                }
                else if (read_buff > 0){
                    while (-1!=(write_buff=(write(dstfp,buffer,BUFSIZE)))){
                        if(write_buff == -1)
                            perror("err in write");
                        if(read_buff == write_buff)
                            break;
                    }
                }
            }
            write_buff = write(dstfp,buffer,read_buff);
            if(write_buff == -1)
                perror("err in write");
        }
        finish = clock();
        timespan = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << timespan * 1000 << "ms with Unix system calls read() and write() with buff" << endl;
        remove(temp_path);

        // time of read and write in char
        start = clock();
        if (-1==(sfp=open(spath,O_RDONLY))){
            perror("err in open src file");
        }
        if (-1==(dstfp=open(temp_path,O_WRONLY|O_CREAT))){
            perror("err in open dst file");
        }
        if(sfp){
            while(0!=(read_buff=read(sfp,chbuff,1))){
                if (read_buff == -1){
                    perror("err in read");
                    break;
                }
                read_buff=write(dstfp,chbuff,1);
                if(write_buff == -1){
                    perror("err in write");
                }
            }
        }
        
        finish = clock();
        timespan = (double)(finish - start) / CLOCKS_PER_SEC;
        cout << timespan * 1000 << "ms with Unix system calls read() and write() with char" << endl; 
    }    
    return;
}

int main(int argc, const char *argv[]){
    int time_flag=0;
    if (argc == 4){
        if (argv[3][0]=='-' && strlen(argv[3])<3){
            if (strstr(argv[3],"t")!=NULL)
                time_flag=1;
            else{
                cout<<"cp: illegal option -- 2"<<endl;
                cout<<"usage: cp [filename1] [filedir2] [-t]"<<endl;
                return -1;
            }
            my_cpfile(argv[1],argv[2],time_flag);
        }
    }
    else if (argc==3)
        my_cpfile(argv[1],argv[2],time_flag);
    else{
        cout << "arguement error" << endl; 
        cout << "usage: cp [filename1] [filedir2] [-t]" << endl;
        return -1;
    }
    cout << "my_copy_file_done~" << endl;
    return 0;    
}
