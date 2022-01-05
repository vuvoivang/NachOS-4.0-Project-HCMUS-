#include "syscall.h"


int main()
{
  char *filename="fileydam.txt";
  char buffer[255];
  int size = Read(filename,255,1);
  Write(buffer,255,0);
  Halt();
}