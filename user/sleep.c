#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  
  if(argc != 2){
    fprintf(2,"参数必须为1\n");
    exit(1);
  } 
  else
  {
    int ticks = atoi(argv[1]);
    int ret = sleep(ticks);
    exit(ret);
  }
  
}
