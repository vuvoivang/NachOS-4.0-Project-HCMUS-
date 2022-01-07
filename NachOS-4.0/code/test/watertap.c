#include "copyright.h"
#include "syscall.h"

void main() {
  int fID_watertap, fID_result; // id of file
  char c;                       // read char from file
  int tap1, tap2;               // water tap 1, water tap 2
  int volume;                   // water bottle volumn of student
  int is_end_student_queue;     // True if serving all student in the queue
  //-----------------------------------------------------------

  tap1 = tap2 = 0;

  while (1) {
    Wait("main_watertap");

    // open file result.txt to write order of water tap
    fID_result = Open("result.txt", ReadWrite);
    if (fID_result == -1) {
      Signal("student");
      return;
    }
    while (1) {
      Wait("watertap");
      c = 0;

      // open file watertap.txt to read volume
      fID_watertap = Open("watertap.txt", ReadOnly);
      if (fID_watertap == -1) {
        Close(fID_result);
        Signal("student");
        return;
      }

      volume = 0;
      is_end_student_queue = 0;

      while (1) {
        // end of file
        if (Read(&c, 1, fID_watertap) == -2) {
          Close(fID_watertap);
          break;
        }
        
        if (c != '*') {
          volume = volume * 10 + (c - '0');
        } else { // end of student queue
          is_end_student_queue = 1;
          Close(fID_watertap);
          break;
        }
      }
      if (volume != 0) {
        // use water tap 1
        if (tap1 <= tap2) {
          tap1 += volume;
          Write("1", 1, fID_result);
        } else // use water tap 2
        {
          tap2 += volume;
          Write("2", 1, fID_result);
        }
      }

      // reset after serving all student in the queue
      if (is_end_student_queue == 1) {
        tap1 = tap2 = 0;
        Close(fID_result);
        Signal("student");
        break;
      }

      Signal("student");
    }
  }
}
