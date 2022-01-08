#include "copyright.h"
#include "syscall.h"

// create 4 semahore
int Create4Semaphore();

// write from result.txt to output.txt
int WriteToOutput(int fID_result, int fID_output, int fID_input);

// write each line of input.txt to student.txt
int WriteToStudent(int fID_input, int fID_output);

// ----- begin main ---------

int main() {

  int fID_input, fID_output, fID_student, fID_result; // id of file
  int num; // number of times considered
  char c;  // char read from file
  //-----------------------------------------------------------

  if (Create4Semaphore() == -1)
    return 1;

  if (CreateFile("output.txt") == -1)
    return 1;

  fID_input = Open("input.txt", ReadOnly);
  if (fID_input == -1)
    return 1;

  fID_output = Open("output.txt", ReadWrite);
  if (fID_output == -1) {
    Close(fID_input);
    return 1;
  }

  // read num
  num = 0;
  while (1) {
    Read(&c, 1, fID_input);
    if (c != '\n') {
      if (c >= '0' && c <= '9')
        num = num * 10 + (c - 48);
    } else
      break;
  }

  // Exec student and watertap process
  if (Exec("../test/student") == -1 || Exec("../test/watertap") == -1) {
    Close(fID_input);
    Close(fID_output);
    return 1;
  }

  while (num--) {
    // write each line of input.txt to student.txt
    WriteToStudent(fID_input, fID_output);

    // switch to student process
    Signal("student");

    // block main process
    Wait("main");

    // write from result.txt to output.txt
    if (WriteToOutput(fID_result, fID_output, fID_input) == -1)
      return 1;
  }

  Close(fID_input);
  Close(fID_output);
  return 0;
}

// ----------------end main---------------------

int Create4Semaphore() {
  if (CreateSemaphore("main", 0) == -1 || CreateSemaphore("student", 0) == -1 ||
      CreateSemaphore("watertap", 0) == -1 ||
      CreateSemaphore("main_watertap", 0) == -1) {
    return -1;
  }
  return 0;
};

int WriteToOutput(int fID_result, int fID_output, int fID_input) {
  char c;
  fID_result = Open("result.txt", ReadOnly);
  if (fID_result == -1) {
    Close(fID_input);
    Close(fID_output);
    return -1;
  }

  while (1) {
    // end of file
    if (Read(&c, 1, fID_result) < 1) {
      Write("\r\n", 2, fID_output);
      Close(fID_result);

      Signal("main_watertap");
      break;
    }
    Write(&c, 1, fID_output);
    Write(" ", 1, fID_output);
  }

  Close(fID_result);
  return 0;
};

int WriteToStudent(int fID_input, int fID_output) {
  char c;
  int fID_student;

  if (CreateFile("student.txt") == -1) {
    Close(fID_input);
    Close(fID_output);
    return 1;
  }

  fID_student = Open("student.txt", ReadWrite);
  if (fID_student == -1) {
    Close(fID_input);
    Close(fID_output);
    return 1;
  }

  while (1) {
    // end of file
    if (Read(&c, 1, fID_input) < 1) {
      break;
    }
    if (c != '\n') {
      Write(&c, 1, fID_student);
    } else
      break;
  }

  Close(fID_student);
}