#include "syscall.h"

int main()
{
  int size;
  char result[255];
  int fInId,fOutId;
  char* buffer = "abcdefgh";

  if(CreateFile("fileTest.txt")==0){
    PrintString("Tao file thanh cong nhe!");
  }
  else{
    PrintString("Tao file that bai r :<");
  }

  // Mo file de ghi buffer vao
  fInId = Open("fileTest.txt",ReadWrite);
  Write(buffer,5,fInId);
  Close(fInId);
  // Mo file de doc
  fOutId = Open("fileTest.txt",ReadOnly);
  // doi den vi tri 1
  Seek(1,fOutId);

  // Doc toi da 255 ky tu
  size = Read(result,255,fOutId);
  
  PrintString("\nSo ky tu doc duoc: ");
  PrintNum(size);
  PrintChar('\n');
  PrintString(result);
  Close(fOutId);
  
  Halt();
}