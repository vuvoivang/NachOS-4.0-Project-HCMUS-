#include "syscall.h"


int main()
{
    int a;
    a=ReadNum();
    PrintString("So vua nhap la ");
    PrintNum(a);
    PrintChar('\n');
    
    Halt();
}