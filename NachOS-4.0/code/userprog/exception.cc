// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "main.h"
#include "copyright.h"
#include "kernel.h"
#include "ksyscall.h"
#include "synchconsole.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------
#define MAX_LENGTH_STRING 2147483647
#define INT_MIN -2147483647
#define INT_MAX 2147483647

#define MAX_LENGTH_FILENAME 32
void increasePC()
{
  /* set previous programm counter (debugging only)*/
  kernel->machine->WriteRegister(PrevPCReg,
                                 kernel->machine->ReadRegister(PCReg));

  /* set programm counter to next instruction (all Instructions are 4 byte
   * wide)*/
  kernel->machine->WriteRegister(PCReg,
                                 kernel->machine->ReadRegister(PCReg) + 4);

  /* set next programm counter for brach execution */
  kernel->machine->WriteRegister(NextPCReg,
                                 kernel->machine->ReadRegister(PCReg) + 4);
}

void clearInputBuffer()
{
  char c;
  while ((c = kernel->synchConsoleIn->GetChar()) != '\n' && c != EOF)
  {
  };
}

char *User2System(int virAddr, int limit)
{
  int i;
  int oneChar;
  char *kernelBuf = NULL;
  kernelBuf = new char[limit + 1];
  if (kernelBuf == NULL)
    return kernelBuf;
  memset(kernelBuf, 0, limit + 1);
  // printf("\n Filename u2s: ");
  for (i = 0; i < limit; i++)
  {
    kernel->machine->ReadMem(virAddr + i, 1,
                             &oneChar); // doc gia tri tu virArr den oneChar
    kernelBuf[i] = (char)oneChar;       // sau do gan vao buffer
    // printf("%c",kernelBuf[i]);
    if (oneChar == 0) // ki tu null cuoi chuoi
      break;
  }
  return kernelBuf;
}

// Input: 	- User space address (int)
// 			- Limit of buffer (int)
// 			- Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, int len, char *buffer)
{
  if (len < 0)
    return -1;
  if (len == 0)
    return len;
  int i = 0;
  int oneChar = 0;
  do
  {
    oneChar = (int)buffer[i];
    kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
    i++;
  } while (i < len && oneChar != 0);
  return i;
}

void ExceptionHandler(ExceptionType which)
{
  int type = kernel->machine->ReadRegister(2);

  DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

  switch (which)
  {
  case NoException:
    return;

  case PageFaultException:
    DEBUG(dbgAddr, "\n khong tim duoc trang phu hop.");
    printf("\n\n khong tim duoc trang phu hop.");
    SysHalt();

    break;

  case ReadOnlyException:
    DEBUG(dbgAddr, "\nloi co gang viet vao trang chi danh cho doc.");
    printf("\n\n loi co gang viet vao trang chi danh cho doc.");
    SysHalt();
    break;

  case BusErrorException:
    DEBUG(dbgAddr, "\n bien dich den dia chi thuc khong hop le.");
    printf("\n\n bien dich den dia chi thuc khong hop le.");
    SysHalt();
    break;

  case AddressErrorException:
    DEBUG(dbgAddr, "\n dia chi truy cap ngoai khong gian vung nho hoac khong duoc danh dau.");
    printf("\n\n dia chi truy cap ngoai khong gian vung nho hoac khong duoc danh dau.");
    SysHalt();
    break;

  case OverflowException:
    DEBUG(dbgAddr, "\nCong tru so nguyen bi vuot qua gioi han.");
    printf("\n\n Cong tru so nguyen bi vuot qua gioi han.");
    SysHalt();
    break;

  case IllegalInstrException:
    DEBUG(dbgAddr, "\n instr chua duoc dinh nghia.");
    printf("\n\n instr chua duoc dinh nghia.");
    SysHalt();
    break;

  case NumExceptionTypes:
    DEBUG(dbgAddr, "\n Khong dung kieu du lieu so");
    printf("\n\n Khong dung kieu du lieu so");
    SysHalt();
    break;
  case SyscallException:
    switch (type)
    {
    case SC_Halt:
      DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
      SysHalt();
      ASSERTNOTREACHED();
      break;

    case SC_Add:
    {
      DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + "
                           << kernel->machine->ReadRegister(5) << "\n");

      /* Process SysAdd Systemcall*/
      int result;
      result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
                      /* int op2 */ (int)kernel->machine->ReadRegister(5));

      DEBUG(dbgSys, "Add returning with " << result << "\n");
      /* Prepare Result */
      kernel->machine->WriteRegister(2, (int)result);

      /* Modify return point */
      increasePC();
      return;

      ASSERTNOTREACHED();

      break;
    }
    case SC_ReadNum:
    {
      //input : None
      //output : So nguyen hoac 0
      long long res; // khai bao ket qua ra ve kieu long long de sau nay ep kieu
      char c;
      bool isNegative;
      bool isEnd, flagZero;
      isEnd = false;
      isNegative = false;
      flagZero = false;
      res = 0;
      while ((c = kernel->synchConsoleIn->GetChar()) == ' ')
      { // kiem tra xem co khoang trang o dau hay khong
      }

      if (c == '-') // check xem co phai so am hay khong
        isNegative = true;
      else if (c >= '0' &&
               c <= '9') // check xem cac ki tu co nam tu 0->9 hay khong
        res = res * 10 + (c - '0');
      else if (c == '\n')
      {
        DEBUG(dbgSys, "\nNguoi dung chua nhap so");
        printf("\nNguoi dung chua nhap so");
        res = 0;
        kernel->machine->WriteRegister(2, int(res));
        increasePC();
        return;
      }
      else
      {
        DEBUG(dbgSys, "\nError!!!Nguoi dung nhap ki tu sai so nguyen");
        res = 0;
        kernel->machine->WriteRegister(2, int(res));
        printf("khong phai so nguyen");
        // while(kernel->synchConsoleIn->GetChar()!='\n')
        // {

        // }
        clearInputBuffer();
        increasePC();
        return;
      }
      while ((c = kernel->synchConsoleIn->GetChar()) != '\n')
      { // doc den luc Enter xuong dong

        if (c >= '0' && c <= '9' && isEnd == false)
        {
          res = res * 10 + (c - '0');
          flagZero = true;
        }
        else if (c == ' ')
        { // neu nhan them khoang trang thi den day la ket thuc
          isEnd = true;
        }
        else
        { // neu la cac ki tu khac thi loi
          DEBUG(dbgSys, "\nError!!!This is not a number");
          res = 0;
          ;
          printf("\nError!!!This is not a number");
          kernel->machine->WriteRegister(2, int(res));

          clearInputBuffer();
          // while(kernel->synchConsoleIn->GetChar()!='\n')
          // {

          // }
          increasePC();

          return;
        }
      }
      if (isNegative)
        res = -res;
      if (res > INT_MAX)
      { // kiem tra so nguyen co lon hon quy dinh hay khong INT MAX 2147483647
        DEBUG(dbgSys, "\nError!!!This number is so large");
        printf("\nSo qua lon");
        res = 0;
        kernel->machine->WriteRegister(2, int(res));

        increasePC();
        return;
      }
      else if (res < INT_MIN)
      { // kiem tra so nguyen co be hon quy dinh hay khong INT MIN -2147483647
        DEBUG(dbgSys, "\nError!!!This number is so small");
        printf("\nSo qua be");
        res = 0;
        kernel->machine->WriteRegister(2, int(res));

        increasePC();
        return;
      }
      if (isNegative && res == 0 && !flagZero)
      { // kiem tra truong hop nguoi dung nhap '-0'
        DEBUG(dbgSys, "\nError!!!This is (-) only");
        res = 0;
        printf("\nKhong phai la so nguyen ma chi la dau (-)");
        kernel->machine->WriteRegister(2, int(res));
        increasePC();
        return;
      }

      DEBUG(dbgSys, "\nThis is a number");
      kernel->machine->WriteRegister(2, int(res));
      //printf("Value: %d", res);
      // cout << "so do la" << res;

      increasePC();
      return;
    }
    case SC_PrintNum:
    {
      // Input: 1 so nguyen kieu int
      // Return: none
      // Tac dung: in mot so nguyen kieu int ra console
      int number;
      int temp;
      char *buffer;
      int countDigits, indexStart, MAX_SIZE_BUFFER;
      int i;
      countDigits = 0; // So digit cua number
      indexStart = 0;  // Vi tri bat dau in chu so

      MAX_SIZE_BUFFER = 11; // 1: sign, 10: digits vi so nguyen kieu int toi da can 10 digits
      number = kernel->machine->ReadRegister(4);

      if (number == 0)
      {                                        // truong hop de xu ly nhat
        kernel->synchConsoleOut->PutChar('0'); // In ra man hinh so 0
        increasePC();
        return;
      }

      /*Chuyen so thanh chuoi roi in ra man hinh*/
      if (number < 0)
      {

        number = -number; // Chuyen thanh so duong => easy
        indexStart = 1;
      }

      temp = number;   // bien tam cho number
      while (temp > 0) // dem so digits
      {
        countDigits++;
        temp /= 10;
      }

      // Tao buffer chuoi de in ra man hinh
      // int: [-2147483648, 2147483647]
      buffer = new char[MAX_SIZE_BUFFER + 1];
      for (i = countDigits - 1 + indexStart; i >= indexStart; i--)
      {
        buffer[i] = (char)((number % 10) + '0'); // Lay tung chu so vao buffer
        number /= 10;
      }
      if (indexStart == 1)
      { // Neu la so am
        buffer[0] = '-';
        buffer[countDigits + 1] = 0;
        for (i = 0; i <= countDigits; i++)
        {
          kernel->synchConsoleOut->PutChar(buffer[i]);
        }

        delete buffer;
      }
      else
      { // Neu la so duong
        buffer[countDigits] = 0;
        for (i = 0; i <= countDigits - 1; i++)
        {
          kernel->synchConsoleOut->PutChar(buffer[i]);
        }

        delete buffer;
      }
      increasePC();

      return;
    }
    case SC_PrintString:
    {
      // input char[]
      //output None
      int virAddr;   // khai bao bien dia chi de doc tu thanh ghi
      char *strName; // ten cua chuoi o phia user space
      int length;    // chieu dai chuoi nguoi dung nhap
      int temp;      // bien tam de luu
      char c;

      virAddr = kernel->machine->ReadRegister(4);
      temp = virAddr;

      length = 0;
      // tinh chieu dai chuoi ma nguoi dung nhap vao
      do
      {
        kernel->machine->ReadMem(temp, 1, (int *)&c);
        length++;
        temp = temp + 1;
      } while (c != '\0');

      strName = User2System(virAddr, length); // truyen du lieu qua kernel space de xu ly

      if (strName == NULL)
      { // kiem tra Truong hop khong du bo nho trong kernel space
        printf("\n Not enough memory in system");
        DEBUG(dbgAddr, "\n Not enough memory in system");
        kernel->machine->WriteRegister(2, -1); // return error
        delete strName;
        increasePC();
        return;
      }
      else
      {
        if (strlen(strName) > MAX_LENGTH_STRING)
        { // kiem tra neu chieu dai chuoi vuot qua quy dinh 1 chuoi cho phep
          printf("\n out of index");

          kernel->machine->WriteRegister(2, -1); // return error
          delete strName;
          increasePC();
          return;
        }
        else if (strlen(strName) == 0)
        { // nguoi dung ko nhap gi
          kernel->synchConsoleOut->PutChar('\0');
          printf("\n chuoi rong");
          DEBUG(dbgAddr, "\n chuoi rong");
          increasePC();
          return;
        }
        else
        {
          for (int i = 0; i < strlen(strName); i++)
          {
            kernel->synchConsoleOut->PutChar(strName[i]);
          }
          //printf("\n In chuoi thanh cong.");
          DEBUG(dbgAddr, "\n In chuoi thanh cong.");
          increasePC();
          return;
        }
      }
      break;
    }
    case SC_ReadString:
    {
      // input char[] voi int length
      //output :None
      int virAddr; // khai bao dia chi nhan tu thanh ghi
      int length;
      int inputLength;
      char *strName;
      char c;

      virAddr = kernel->machine->ReadRegister(4); // lay dia chi tu thanh ghi (char buffer[] o user space)
      length = kernel->machine->ReadRegister(5);  // lay dia chi tu thanh
      //cout<<"length "<<length<<" ";
      strName = new char[length]; // day se la bien buffer duoc tra ve cho nguoi dung
      inputLength = 0;
      while ((c = kernel->synchConsoleIn->GetChar()) != '\n')
      {
        strName[inputLength] = c;
        inputLength++;
      }
      //cout<<"strName la "<<strName;
      strName[inputLength] = '\0';

      int numBytes = System2User(virAddr, inputLength, strName); // chuyen bo nho qua user
      //cout<<"numbyte "<<numBytes;
      if (inputLength > length)
      {
        printf("\nChuoi nhap co do dai qua lon so voi quy dinh mat roi\n");
        //cout << strName;
        increasePC();
        return;
      }
      if (numBytes == 0)
      {
        printf("Chuoi rong\n");
      }
      else if (numBytes > MAX_LENGTH_STRING)
      {
        printf("\nChuoi qua lon so vs he thong");
        increasePC();
        return;
      }

      //clearInput();
      increasePC();
      return;
    }
    case SC_RandomNum:
    {
      // input: none
      // ouput: mot so nguyen duong ngau nhien
      int result;
      RandomInit(time(NULL));
      result = RandomNumber();
      kernel->machine->WriteRegister(2, result);
      increasePC();
      return;
    }
    case SC_ReadChar:
    {
      // input: none
      // output: mot ki tu char
      char result;
      char c = 0;

      result = kernel->synchConsoleIn->GetChar();
      if (result == '\n') // nguoi dung khong nhap
      {
        DEBUG(dbgSys, "\nERROR: Ban nhap ky tu rong!");
        printf("\nERROR: Ban nhap ky tu rong!");
      }
      else // check xem co phai dung 1 ky tu khong?
      {
        c = kernel->synchConsoleIn->GetChar();
        if (c != '\n')
        {
          result = 0;
          DEBUG(dbgSys, "\nERROR: Ban nhap nhieu hon 1 ky tu!");
          printf("\nERROR: Ban nhap nhieu hon 1 ky tu!");
          clearInputBuffer();
        }
      }
      kernel->machine->WriteRegister(2, result);
      increasePC();
      return;
    }
    case SC_PrintChar:
    {
      // input: 1 char
      // output: none
      // muc dich: in ra console 1 ki tu char
      char c = kernel->machine->ReadRegister(4);
      kernel->synchConsoleOut->PutChar(c);
      increasePC();
      return;
    }
    // project 2
    case SC_CreateFile:
    {
      // input: dia chi cua filename o userspace
      // output: 0 = thanh cong, -1 = loi
      // muc dich: tao file voi filename duoc truyen vao
      int virAddr;
      char *filename;
      DEBUG(dbgFile,"\n SC_CreateFile call ...");
      DEBUG(dbgFile,"\n Reading virtual address of filename");

      virAddr = kernel->machine->ReadRegister(4);
      DEBUG (dbgFile,"\n Reading filename.");
      filename = User2System(virAddr,MAX_LENGTH_FILENAME);
      if(filename == NULL){
        printf("\n Not enough memory in system");
        DEBUG(dbgFile,"\n Not enough memory in system");
        kernel->machine->WriteRegister(2,-1); // tra ve Loi cho chuong trinh nguoi dung
        increasePC();
        return;
      }
      DEBUG(dbgFile,"\n Finish reading filename.");

      // create file
      if(!fileSystem->Create(filename)){
        printf("\n Error create file '%s'",filename);
        kernel->machine->WriteRegister(2,-1);
        delete filename;
        increasePC();
        return;
      }

      // create file thanh cong
      kernel->machine->WriteRegister(2,0);
      delete filename;
      increasePC();
      return;
    }

    default:
      cerr << "Unexpected system call " << type << "\n";
      break;
    }
    break;
  default:
    cerr << "Unexpected user mode exception" << (int)which << "\n";
    break;
  }
  ASSERTNOTREACHED();
}
