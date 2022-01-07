#include "copyright.h"
#include "syscall.h"

#define MAX_LENGTH 32

int Create4Semaphore() {
  if (CreateSemaphore("main", 0) == -1 ||
      CreateSemaphore("sinhvien", 0) == -1 ||
      CreateSemaphore("voinuoc", 0) == -1 || CreateSemaphore("m_vn", 0) == -1) {
    return -1;
  }
  return 0;
};

int WriteOutput(int fID_result, int fID_output, int fID_input) {
  char c;

  fID_result = Open("result.txt", ReadOnly);
  if (fID_result == -1) {
    Close(fID_input);
    Close(fID_output);
    return -1;
  }
  while (1) {
    if (Read(&c, 1, fID_result) < 1) {
      Write("\r\n", 2, fID_output);
      Close(fID_result);
      Signal("m_vn");
      break;
    }
    Write(&c, 1, fID_output);
    Write(" ", 1, fID_output);
  }
  Close(fID_result);
  return 0;
};

int WriteSinhVien(int fID_input, int fID_output) {
  char c;
  int fID_sinhvien;
  // Tao file sinhvien.txt
  if (CreateFile("sinhvien.txt") == -1) {
    Close(fID_input);
    Close(fID_output);
    return 1;
  }

  // Mo file sinhvien.txt de ghi tung dong sinhvien tu file input.txt
  fID_sinhvien = Open("sinhvien.txt", ReadWrite);
  if (fID_sinhvien == -1) {
    Close(fID_input);
    Close(fID_output);
    return 1;
  }

  while (1) {
    if (Read(&c, 1, fID_input) < 1) {
      // Doc toi cuoi file
      break;
    }
    if (c != '\n') {
      Write(&c, 1, fID_sinhvien);
    } else
      break;
  }
  // Dong file sinhvien.txt lai
  Close(fID_sinhvien);
}

int main() {

  int f_Success; // Bien co dung de kiem tra thanh cong
  int fID_input, fID_output, fID_sinhvien, fID_result; // Bien id cho file
  int num;                                        // Luu so luong thoi diem xet
  char c; // Bien ki tu luu ki tu doc tu file
  //-----------------------------------------------------------

  // Khoi tao 4 Semaphore de quan ly 3 tien trinh
  if (Create4Semaphore() == -1)
    return 1;

  // Tao file output.txt de ghi ket qua cuoi cung
  if (CreateFile("output.txt") == -1)
    return 1;

  // Mo file input.txt chi de doc
  fID_input = Open("input.txt", ReadOnly);
  if (fID_input == -1)
    return 1;

  // Mo file output.txt de doc va ghi
  fID_output = Open("output.txt", ReadWrite);
  if (fID_output == -1) {
    Close(fID_input);
    return 1;
  }

  // Doc so luong thoi diem xet o file input.txt
  //**** Thuc hien xong doan lenh duoi thi con tro file o input.txt o dong 1
  num = 0;
  while (1) {
    Read(&c, 1, fID_input);
    if (c != '\n') {
      if (c >= '0' && c <= '9')
        num = num * 10 + (c - 48);
    } else
      break;
  }

  // Goi thuc thi tien trinh sinhvien.c
  if (Exec("../test/sinhvien") == -1 || Exec("../test/voinuoc") == -1) {
    Close(fID_input);
    Close(fID_output);
    return 1;
  }

  // Thuc hien xu ly khi nao het thoi diem xet thi thoi
  while (num--) {
    WriteSinhVien(fID_input, fID_output);
    // Goi tien trinh sinhvien hoat dong
    Signal("sinhvien");

    // Tien trinh chinh phai cho
    Wait("main");

    // Thuc hien doc file tu result va ghi vao ket qua o output.txt

    // write from result.txt to output.txt
    if (WriteOutput(fID_result, fID_output, fID_input) == -1)
      return 1;
  }

  Close(fID_input);
  Close(fID_output);
  return 0;
}
