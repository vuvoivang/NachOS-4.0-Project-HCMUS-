#include "syscall.h"

int main()
{
    char *filename;
    char*str;
    int type;
    int file_id;
    filename="fileydam.txt";
    type=1;
    file_id=3;
    
    // file_id=Open(filename,type);
    // if(file_id!=-1){
    //     PrintString("Mo file thanh cong nhe!");
       
    // }
    // else{
    //      PrintString("Mo file that bai nhe!");
    // }
    // PrintNum(file_id);

    if(Close(file_id)!=-1){
        PrintString("Dong File thanh cong");
    }
    else{
        PrintString("Dong file khong thanh cong");
    }
    Halt();
}