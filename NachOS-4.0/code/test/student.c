#include "copyright.h"
#include "syscall.h"

void main() {
  int fID_student, fID_watertap;// id of file
  char c;                       // read char from file
  int flag_watertap;            // flag to back to watertap process
  int file_length;              // file length
  int pos;                      // position of pointer in the file
  //-----------------------------------------------------------
  Signal("main_watertap");

  if (CreateFile("watertap.txt") == -1) {
    Signal("main"); // back to main process
    return;
  }
	
  while (1) {
    file_length = 0;

    Wait("student");

    // create file result.txt to write order of water tap
    if (CreateFile("result.txt") == -1) {
      Signal("main"); // back to main process
      return;
    }

    fID_student = Open("student.txt", ReadOnly);
    if (fID_student == -1) {
      Signal("main"); // back to main process
      return;
    }

    file_length = Seek(-1, fID_student);
    Seek(0, fID_student);
    pos = 0;

    // open file watertap.txt to write water bottle volumn of each student
    fID_watertap = Open("watertap.txt", ReadWrite);
    if (fID_watertap == -1) {
      Close(fID_student);
      Signal("main"); // back to main process
      return;
    }

    while (pos < file_length) {
      flag_watertap = 0;
      Read(&c, 1, fID_student);

      if (c != ' ') {
        Write(&c, 1, fID_watertap);
      }
      else {
        flag_watertap = 1;
      }

      // mark end of queue
      if (pos == file_length - 1) {
        Write("*", 1, fID_watertap);
        flag_watertap = 1;
      }

      if (flag_watertap == 1) {
        Close(fID_watertap);

        Signal("watertap");
        // block student process for executing watertap process
        Wait("student");

        if (CreateFile("watertap.txt") == -1) {
          Close(fID_student);
          Signal("main"); // back to main process
          return;
        }

        fID_watertap = Open("watertap.txt", ReadWrite);
        if (fID_watertap == -1) {
          Close(fID_student);
          Signal("main"); // back to main process
          return;
        }
      }

      pos++;
    }
    
		Close(fID_watertap);
    Close(fID_student);
    Signal("main"); // back to main process
  }
}
