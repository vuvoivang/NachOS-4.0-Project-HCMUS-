#include "syscall.h"


int main()
{
 
  char buffer[255];
  int size = Read(buffer,255,1);
  PrintNum(size);
  PrintChar("\n");
  Write(buffer,255,0);
  Halt();
}