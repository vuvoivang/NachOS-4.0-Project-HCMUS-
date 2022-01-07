#include "syscall.h"

int main()
{
  int size;
  char result[3];
  int fInId,fOutId;
  char* buffer = "abc";
  if(CreateFile("fileYDam.txt")==0){
    PrintString("Tao file thanh cong nhe!");
  }
  else{
    PrintString("Tao file that bai r :<");
  }
  fInId = Open("fileYDam.txt",2);
  Write(buffer,3,fInId);
  Close(fInId);
  fOutId = Open("fileYDam.txt",3);
  Seek(1,fOutId);
  size = Read(result,1,fOutId);
  PrintChar('\n');
  PrintNum(fInId);
  PrintChar('\n');
  PrintNum(size);
  PrintChar('\n');
  PrintString(result);
  Close(fOutId);
  // PrintChar('\n');
  Halt();
}