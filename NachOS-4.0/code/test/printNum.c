#include"syscall.h"

int main()
{
    PrintNum(0);
    PrintChar('\n');
    PrintNum(-2147483647);
    PrintChar('\n');
    PrintNum(123456);
    PrintChar('\n');
    Halt();
}