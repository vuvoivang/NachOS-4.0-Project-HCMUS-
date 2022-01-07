#include "copyright.h"
#include "syscall.h"

void main() {
  // Khai bao
  int f_Success;               // Bien co dung de kiem tra thanh cong
  int fID_sinhvien, fID_voinuoc; // Bien id cho file
  char c;             // Bien ki tu luu ki tu doc tu file
  int flag_VN;                 // Bien co de nhay den tien trinh voinuoc
  int flag_MAIN;               // Bien co de nhay den tien trinh main
  int lengthFile;              // Luu do dai file
  int i_File;                  // Luu con tro file
  //-----------------------------------------------------------
  Signal("m_vn");

  // Tao file voinuoc.txt
  if (CreateFile("voinuoc.txt") == -1) {
    Signal("main"); // tro ve tien trinh chinh
    return;
  }
	
  while (1) {
    lengthFile = 0;

    Wait("sinhvien");

    // Tao file result.txt de ghi voi nao su dung
    if (CreateFile("result.txt") == -1) {
      Signal("main"); // tro ve tien trinh chinh
      return;
    }

    // Mo file sinhvien.txt len de doc
    fID_sinhvien = Open("sinhvien.txt", ReadOnly);
    if (fID_sinhvien == -1) {
      Signal("main"); // tro ve tien trinh chinh
      return;
    }

    lengthFile = Seek(-1, fID_sinhvien);
    Seek(0, fID_sinhvien);
    i_File = 0;

    // Mo file voinuoc.txt de ghi tung dung tich nuoc cua sinhvien
    fID_voinuoc = Open("voinuoc.txt", ReadWrite);
    if (fID_voinuoc == -1) {
      Close(fID_sinhvien);
      Signal("main"); // tro ve tien trinh chinh
      return;
    }

    // Ghi dung tich vao file voinuoc.txt tu file sinhvien.txt
    while (i_File < lengthFile) {
      flag_VN = 0;
      Read(&c, 1, fID_sinhvien);
      if (c != ' ') {
        Write(&c, 1, fID_voinuoc);
      }

      else {
        flag_VN = 1;
      }
      if (i_File == lengthFile - 1) {
        Write("*", 1, fID_voinuoc);
        flag_VN = 1;
      }

      if (flag_VN == 1) {
        Close(fID_voinuoc);
        Signal("voinuoc");
        // Dung chuong trinh sinhvien lai de voinuoc thuc thi
        Wait("sinhvien");

        // Tao file voinuoc.txt
        if (CreateFile("voinuoc.txt") == -1) {
          Close(fID_sinhvien);
          Signal("main"); // tro ve tien trinh chinh
          return;
        }

        // Mo file voinuoc.txt de ghi tung dung tich nuoc cua sinhvien
        fID_voinuoc = Open("voinuoc.txt", ReadWrite);
        if (fID_voinuoc == -1) {
          Close(fID_sinhvien);
          Signal("main"); // tro ve tien trinh chinh
          return;
        }
      }

      i_File++;
    }
    // Ket thuc tien trinh sinhvien va voinuoc quay lai ham SvVn

		Close(fID_voinuoc);
    Close(fID_sinhvien);
    Signal("main");
  }
  // Quay lai ham Svvn
}
