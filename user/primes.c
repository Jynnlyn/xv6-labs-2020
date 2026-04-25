#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void sieve(int p_left){
    //printf("test");
    int first_prime;
    if(read(p_left,&first_prime,sizeof first_prime) == 0){
        //printf("test");
        close(p_left);
        exit(0);
    }
    printf("prime %d\n",first_prime);
    //printf("test");
    int next_pipe[2];
    int num;
    pipe(next_pipe);
    int pid = fork();

    if(pid > 0){
        close(next_pipe[0]);
        while(read(p_left,&num,sizeof(num)) > 0){
            if(num % first_prime != 0){
                    write(next_pipe[1],&num,sizeof num);
            }   
        }
        close(next_pipe[1]);
        close(p_left);
        wait(0);
        exit(0);
    }else{
        close(next_pipe[1]);
        close(p_left);
        sieve(next_pipe[0]);
    }
}

int main(int argc,char *argv[]){

    int p[2];
    pipe(p);
    int pid = fork();

    if(pid > 0){
        close(p[0]);
        for(int i = 2;i <= 35; i ++){
            write(p[1],&i,sizeof(i));
        }
        close(p[1]);
        wait(0);
        exit(0);

    }else{
        close(p[1]);
        sieve(p[0]);
    }

    exit(0);
}