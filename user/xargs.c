#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc,char * argv[])
{
    if (argc < 2) 
    {
        fprintf(2, "Usage: xargs[command]\n");
        exit(1);
    }

    char *exec_argv[MAXARG];
    int exec_argc = 0;

    for(int i = 1; i < argc;i ++) //不用存xargs，从其后面的第一个参数开始
    {
        exec_argv[exec_argc] = argv[i];
        exec_argc++;
    }

    char buf[512];  // 缓冲区：用来存放从标准输入读到的一行字符
    int buf_idx = 0;
    char ch;
    
    //shell会自动把xargs左边的命令结果输出到屏幕上
    while(read(0,&ch,sizeof ch) == 1) //每一行都会执行一次命令
    {
        if(ch != '\n')
        {
            buf[buf_idx ++] = ch;
            if (buf_idx >= 511) 
            {
                buf_idx = 511; 
            }
        }else
        {
            buf[buf_idx] = '\0'; // 给字符串加上结尾符，变成合法的 C 字符串
            exec_argv[exec_argc] = buf;
            // exec 的参数数组必须以 0 (NULL) 结尾！
            exec_argv[exec_argc + 1] = 0; 
            if(fork() == 0)
            {
                exec(exec_argv[0],exec_argv);
                // 如果 exec 执行成功，代码就不会往下走了。
                // 如果能走到这里，说明 exec 失败了（比如找不到该命令）
                fprintf(2, "exec %s failed\n", exec_argv[0]);
                exit(1);
            }else
            {
                wait(0);
            }
            buf_idx = 0;

        }
        
    }
    if(buf_idx > 0) //若最后一行不是换行符进行特判
    {
        buf[buf_idx] = '\0'; // 给字符串加上结尾符，变成合法的 C 字符串
        exec_argv[exec_argc] = buf;
        // 重要：exec 的参数数组必须以 0 (NULL) 结尾！
        exec_argv[exec_argc + 1] = 0; 
        if(fork() == 0)
        {
            exec(exec_argv[0],exec_argv);
            // 如果 exec 执行成功，代码就不会往下走了。
            // 如果能走到这里，说明 exec 失败了（比如找不到该命令）
            fprintf(2, "exec %s failed\n", exec_argv[0]);
            exit(1);
        }else
        {
            wait(0);
        }
        buf_idx = 0;
    }
    
    exit(0);
}