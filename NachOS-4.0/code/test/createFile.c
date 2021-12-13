#include "syscall.h"


int main()
{
  char *filename="taofile_aloalo.txt";
  
  if(CreateFile(filename)==0){
    PrintString("Tao file thanh cong nhe!");
  }
  else{
    PrintString("Tao file that bai r :<");
  }


  Halt();
}