#include "syscall.h"

void main()
{
    int idProcess;
    idProcess = Exec("./test/add");
    PrintString("hello");
    Halt();
}