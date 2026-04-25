#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc,char *argv[]){
    
    int p1[2]; //子 -> 父
    int p2[2]; //父 -> 子
    char buf[] = {'a'};

    pipe(p1);
    pipe(p2);

    int pid = fork();
    if(pid > 0){
        close(p2[0]);
        close(p1[1]);
        write(p2[1],buf,1);
        read(p1[0],buf,1);
        printf("%d: received pong\n", getpid());
        close(p2[1]);
        close(p1[0]);
        exit(0);
    }else if(pid == 0){
        close(p2[1]);
        close(p1[0]);
        read(p2[0],buf,1);
        printf("%d: received ping\n", getpid());
        write(p1[1],buf,1);
        close(p2[0]);
        close(p1[1]);
        exit(0);
    }else{
        fprintf(2,"fork 失败\n");
        exit(1);
    }

    exit(0);
}