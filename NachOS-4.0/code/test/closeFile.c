#include "syscall.h"

int main()
{
    char *filename;
    
    filename="fileydam.txt";
    if(Close(filename)!=-1){
        PrintString("Dong File thanh cong!");
       

    }
    else{
         PrintString("Khong the dong file!");
    }
    Halt();
}