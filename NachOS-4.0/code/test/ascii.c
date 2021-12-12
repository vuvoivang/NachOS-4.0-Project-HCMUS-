#include"syscall.h"

int main()
{   
    int i;
    char c;
    // Ky tu doc duoc: ma ascii tu 32 -> 127
    i = 32; // ky tu in ra man hinh doc duoc bat dau o 32
    PrintString("ASCII Table: \n");
    while (i <= 127) 
    {
        c = i;
        PrintChar(c);
        PrintChar(' ');
        i++;
        if( i%4 == 0) PrintChar('\n'); // 1 dong in 4 ky tu
    }
    Halt();
}