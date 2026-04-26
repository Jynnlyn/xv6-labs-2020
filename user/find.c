#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path,char * file_name)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0)
  {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0)
  {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  switch(st.type)
  {
     case T_FILE:
     {
      char *sub_name;
      sub_name = path + strlen(path); // 指向字符串末尾 \0
      while(sub_name >= path && *sub_name != '/') 
      {
          sub_name--; // 从后往前退，直到遇到斜杠或者退到开头
      }
      sub_name++; // 往前挪一格，刚好指向纯文件名

      // 2. 比对名字
      if (strcmp(sub_name, file_name) == 0) 
      {
          printf("%s\n", path);
      }
      break;
    }
          
    case T_DIR:
    {
      // 检查拼接后的路径长度是否会超过 buf 的容量。
      // 这里预留了路径字符串、斜杠、目录项名以及结尾的空字符所需空间。
      // 如果路径过长，则打印错误信息并终止当前处理，以避免缓冲区溢出。
      if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
      {
        printf("ls: path too long\n");
      }
      strcpy(buf, path);
      p = buf+strlen(buf);
      *p++ = '/';
      while(read(fd, &de, sizeof(de)) == sizeof(de))
      { 
        if(de.inum == 0) // inode为0代表这个文件被删除了，跳过
          continue;
        memmove(p, de.name, DIRSIZ); //把文件名拼接到路径后面
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0)
        { 
          printf("find: cannot stat %s\n", buf);
          continue;
        }else if (st.type == T_FILE && !strcmp(de.name,file_name))
        {
          printf("%s\n",buf);
        }else if (st.type == T_DIR)
        {
          if((!strcmp(de.name,".") )|| (!strcmp(de.name,".."))) continue;
          find(buf,file_name);
        }
      }
      break;
    }
      
  }
 

  close(fd);
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
      fprintf(2,"usage: find [path] [target_filename]\n");
    }else
    {
      find(argv[1],argv[2]);
    }

    exit(0);
}
