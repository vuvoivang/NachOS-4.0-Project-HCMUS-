#include"syscall.h"

int main()
{   
    // Thong tin nhom
    PrintString("Thong tin nhom: 3 thanh vien \n");
    PrintString("  -Thanh vien 1: Dam Thi Xuan Y, Ma so sinh vien: 19120160 \n");
    PrintString("  -Thanh vien 2: Nguyen Le Quang, Ma so sinh vien: 19120121 \n");
    PrintString("  -Thanh vien 3: Vo Hoang Vu, Ma so sinh vien: 19120727 \n");

    // Mo ta chuong trinh ascii
    PrintString("Mo ta chuong trinh ascii: \n");
    PrintString("  -Dung mot bien kieu char lan luot in cac ky tu doc duoc (32 - 127), in 1 dong 4 ky tu \n");
    PrintString("  -In mot ky bang system call PrintChar da cai dat \n");

    // Mo ta chuong trinh my_sort
    PrintString("Mo ta chuong trinh my_sort: \n");
    PrintString("  -Nhap n bang system call ReadNum, dung while xu li dieu kien n thuoc [1,100] \n");
    PrintString("  -Tiep theo, nhap n moi phan tu cua Array bang ReadNum \n");
    PrintString("  -Bien isIncrease: lua chon sap xep tang dan (nhap 1) hay giam dan (nhap 2), neu khong nhap 1 va 2 thi bat nhap lai \n");
    PrintString("  -Truyen isIncrease vao bubbleSort de sap xep theo lua chon \n");
    PrintString("  -In ra man hinh ket qua \n");
    
    Halt();
}