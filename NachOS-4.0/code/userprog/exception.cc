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

#include "copyright.h"
#include "kernel.h"
#include "ksyscall.h"
#include "main.h"
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
#define RW_ERROR_FILE -1
#define RW_END_FILE -2

void increasePC() {
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

void clearInputBuffer() {
  char c;
  while ((c = kernel->synchConsoleIn->GetChar()) != '\n' && c != EOF) {
  };
}

char *User2System(int virAddr, int limit) {
  int i;
  int oneChar;
  char *kernelBuf = NULL;
  kernelBuf = new char[limit + 1];
  if (kernelBuf == NULL)
    return kernelBuf;
  memset(kernelBuf, 0, limit + 1);
  // printf("\n Filename u2s: ");
  for (i = 0; i < limit; i++) {
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
int System2User(int virtAddr, int len, char *buffer) {
  if (len < 0)
    return -1;
  if (len == 0)
    return len;
  int i = 0;
  int oneChar = 0;
  do {
    oneChar = (int)buffer[i];
    kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
    i++;
  } while (i < len && oneChar != 0);
  return i;
}

void ExceptionHandler(ExceptionType which) {
  int type = kernel->machine->ReadRegister(2);
  DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");
  FileSystem* fileSystem = pTab->getFileTable(kernel->currentThread->processID);
  

  switch (which) {
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
    DEBUG(dbgAddr, "\n dia chi truy cap ngoai khong gian vung nho hoac khong "
                   "duoc danh dau.");
    printf("\n\n dia chi truy cap ngoai khong gian vung nho hoac khong duoc "
           "danh dau.");
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
    switch (type) {
    case SC_Halt:
      DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
      SysHalt();
      ASSERTNOTREACHED();
      break;

    case SC_Add: {
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
    case SC_ReadNum: {
      // input : None
      // output : So nguyen hoac 0
      long long res; // khai bao ket qua ra ve kieu long long de sau nay ep kieu
      char c;
      bool isNegative;
      bool isEnd, flagZero;
      isEnd = false;
      isNegative = false;
      flagZero = false;
      res = 0;
      while ((c = kernel->synchConsoleIn->GetChar()) ==
             ' ') { // kiem tra xem co khoang trang o dau hay khong
      }

      if (c == '-') // check xem co phai so am hay khong
        isNegative = true;
      else if (c >= '0' &&
               c <= '9') // check xem cac ki tu co nam tu 0->9 hay khong
        res = res * 10 + (c - '0');
      else if (c == '\n') {
        DEBUG(dbgSys, "\nNguoi dung chua nhap so");
        printf("\nNguoi dung chua nhap so");
        res = 0;
        kernel->machine->WriteRegister(2, int(res));
        increasePC();
        return;
      } else {
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
      while ((c = kernel->synchConsoleIn->GetChar()) !=
             '\n') { // doc den luc Enter xuong dong

        if (c >= '0' && c <= '9' && isEnd == false) {
          res = res * 10 + (c - '0');
          flagZero = true;
        } else if (c ==
                   ' ') { // neu nhan them khoang trang thi den day la ket thuc
          isEnd = true;
        } else { // neu la cac ki tu khac thi loi
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
      if (res > INT_MAX) { // kiem tra so nguyen co lon hon quy dinh hay khong
                           // INT MAX 2147483647
        DEBUG(dbgSys, "\nError!!!This number is so large");
        printf("\nSo qua lon");
        res = 0;
        kernel->machine->WriteRegister(2, int(res));

        increasePC();
        return;
      } else if (res < INT_MIN) { // kiem tra so nguyen co be hon quy dinh hay
                                  // khong INT MIN -2147483647
        DEBUG(dbgSys, "\nError!!!This number is so small");
        printf("\nSo qua be");
        res = 0;
        kernel->machine->WriteRegister(2, int(res));

        increasePC();
        return;
      }
      if (isNegative && res == 0 &&
          !flagZero) { // kiem tra truong hop nguoi dung nhap '-0'
        DEBUG(dbgSys, "\nError!!!This is (-) only");
        res = 0;
        printf("\nKhong phai la so nguyen ma chi la dau (-)");
        kernel->machine->WriteRegister(2, int(res));
        increasePC();
        return;
      }

      DEBUG(dbgSys, "\nThis is a number");
      kernel->machine->WriteRegister(2, int(res));

      increasePC();
      return;
    }
    case SC_PrintNum: {
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

      MAX_SIZE_BUFFER =
          11; // 1: sign, 10: digits vi so nguyen kieu int toi da can 10 digits
      number = kernel->machine->ReadRegister(4);

      if (number == 0) {                       // truong hop de xu ly nhat
        kernel->synchConsoleOut->PutChar('0'); // In ra man hinh so 0
        increasePC();
        return;
      }

      /*Chuyen so thanh chuoi roi in ra man hinh*/
      if (number < 0) {

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
      for (i = countDigits - 1 + indexStart; i >= indexStart; i--) {
        buffer[i] = (char)((number % 10) + '0'); // Lay tung chu so vao buffer
        number /= 10;
      }
      if (indexStart == 1) { // Neu la so am
        buffer[0] = '-';
        buffer[countDigits + 1] = 0;
        for (i = 0; i <= countDigits; i++) {
          kernel->synchConsoleOut->PutChar(buffer[i]);
        }

        delete buffer;
      } else { // Neu la so duong
        buffer[countDigits] = 0;
        for (i = 0; i <= countDigits - 1; i++) {
          kernel->synchConsoleOut->PutChar(buffer[i]);
        }

        delete buffer;
      }
      increasePC();

      return;
    }
    case SC_PrintString: {
      // input char[]
      // output None

      int virtAddr;
			char* buffer;
			virtAddr = kernel->machine->ReadRegister(4); // Lay dia chi cua tham so buffer tu thanh ghi so 4
			buffer = User2System(virtAddr, 255); // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai 255 ki tu
			int length = 0;
			while (buffer[length] != 0) length++; // Dem do dai that cua chuoi
			kernel->synchConsoleOut->Write(buffer, length + 1); // Goi ham Write cua SynchConsole de in chuoi
      delete buffer; 
      increasePC();
      return;
    }
    case SC_ReadString: {
      // input char[] voi int length
      // output :None
      int virAddr; // khai bao dia chi nhan tu thanh ghi
      int length;
      int inputLength;
      char *strName;
      char c;

      virAddr = kernel->machine->ReadRegister(
          4); // lay dia chi tu thanh ghi (char buffer[] o user space)
      length = kernel->machine->ReadRegister(5); // lay dia chi tu thanh
      strName =
          new char[length]; // day se la bien buffer duoc tra ve cho nguoi dung
      inputLength = 0;
      while ((c = kernel->synchConsoleIn->GetChar()) != '\n') {
        strName[inputLength] = c;
        inputLength++;
      }
      strName[inputLength] = '\0';

      int numBytes =
          System2User(virAddr, inputLength, strName); // chuyen bo nho qua user
      if (inputLength > length) {
        printf("\nChuoi nhap co do dai qua lon so voi quy dinh mat roi\n");
        increasePC();
        return;
      }
      if (numBytes == 0) {
        printf("Chuoi rong\n");
      } else if (numBytes > MAX_LENGTH_STRING) {
        printf("\nChuoi qua lon so vs he thong");
        increasePC();
        return;
      }

      // clearInput();
      increasePC();
      return;
    }
    case SC_RandomNum: {
      // input: none
      // ouput: mot so nguyen duong ngau nhien
      int result;
      RandomInit(time(NULL));
      result = RandomNumber();
      kernel->machine->WriteRegister(2, result);
      increasePC();
      return;
    }
    case SC_ReadChar: {
      // input: none
      // output: mot ki tu char
      char result;
      char c = 0;

      result = kernel->synchConsoleIn->GetChar();
      if (result == '\n') // nguoi dung khong nhap
      {
        DEBUG(dbgSys, "\nERROR: Ban nhap ky tu rong!");
        printf("\nERROR: Ban nhap ky tu rong!");
      } else // check xem co phai dung 1 ky tu khong?
      {
        c = kernel->synchConsoleIn->GetChar();
        if (c != '\n') {
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
    case SC_PrintChar: {
      // input: 1 char
      // output: none
      // muc dich: in ra console 1 ki tu char
      char c = (char)kernel->machine->ReadRegister(4); // Doc ki tu tu thanh ghi r4
			kernel->synchConsoleOut->Write(&c, 1); // In ky tu tu bien c, 1 byte
			
      increasePC();
      return;
    }
    // project 2
    case SC_CreateFile: {
      // input: dia chi cua filename o userspace
      // output: 0 = thanh cong, -1 = loi
      // muc dich: tao file voi filename duoc truyen vao
      int virAddr;
      char *filename;
      DEBUG(dbgFile, "\n SC_CreateFile call ...");
      DEBUG(dbgFile, "\n Reading virtual address of filename");

      virAddr = kernel->machine->ReadRegister(4);
      DEBUG(dbgFile, "\n Reading filename.");
      filename = User2System(virAddr, MAX_LENGTH_FILENAME);
      if (filename == NULL) {
        printf("\n Not enough memory in system");
        DEBUG(dbgFile, "\n Not enough memory in system");
        kernel->machine->WriteRegister(
            2, -1); // tra ve Loi cho chuong trinh nguoi dung
        increasePC();
        return;
      }
      DEBUG(dbgFile, "\n Finish reading filename.");

      // create file
      if (!fileSystem->Create(filename)) {
        printf("\n Error create file '%s'", filename);
        kernel->machine->WriteRegister(2, -1);
        delete filename;
        increasePC();
        return;
      }

      // create file thanh cong
      kernel->machine->WriteRegister(2, 0);
      delete filename;
      increasePC();
      return;
    }
    case SC_Open: {
      OpenFile *file;
      int virAddr;
      int type;
      char *fileName;
      int freeSlot;
      virAddr = kernel->machine->ReadRegister(4);
      type = kernel->machine->ReadRegister(5);

      fileName = User2System(virAddr, MAX_LENGTH_FILENAME);

      freeSlot = fileSystem->FindFreeSlot();
      if (type == INPUT_TYPE) {
        // input console stdin ->system doc gia tri tren man hinh console
        // thong bao day
        kernel->machine->WriteRegister(2, 0);
      } else if (type == OUTPUT_TYPE) {
        // thong bao
        // output console stdout -> system xuat ra cho nguoi dung xem tren
        // console
        kernel->machine->WriteRegister(2, 1);
      }
      else if (freeSlot != -1) {
        file = fileSystem->Open(fileName, type);
       
        if (file != NULL) {
          fileSystem->fileTable[freeSlot] = file;
          kernel->machine->WriteRegister(2, freeSlot);
        }
        delete[] fileName;
        increasePC();
        return;
      }
      kernel->machine->WriteRegister(2, -1);
      // print ko tim dc
      delete[] fileName;
      increasePC();
      break;
    }
    case SC_Close: {
      int file_Id;
      file_Id = -1;
      file_Id = kernel->machine->ReadRegister(4);

      if (file_Id >= 2 && file_Id <= MAX_FILE_OPEN-1) {
        if (fileSystem->fileTable[file_Id] != NULL) {

          delete fileSystem->fileTable[file_Id];

          fileSystem->fileTable[file_Id] = NULL;

          kernel->machine->WriteRegister(2, 0);

          increasePC();
          return;
        } else {
          kernel->machine->WriteRegister(2, -1);
          increasePC();
          return;
        }
      }
      // xet them 2 TH 0,1 roi thong bao
      kernel->machine->WriteRegister(2, -1);
      increasePC();
      break;
    }
    case SC_Seek:
		{
			// Input: Vi tri(int), id cua file(OpenFileID)
			// Output: -1: Loi, Vi tri thuc su: Thanh cong
			// Cong dung: Di chuyen con tro den vi tri thich hop trong file voi tham so la vi tri can chuyen va id cua file
			int pos = kernel->machine->ReadRegister(4); // Lay vi tri can chuyen con tro den trong file
			int id = kernel->machine->ReadRegister(5); // Lay id cua file
			// Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
			if (id < 0 || id >= MAX_FILE_OPEN)
			{
				printf("\nKhong the seek vi id nam ngoai bang mo ta file.");
				kernel->machine->WriteRegister(2, -1);
				increasePC();
				return;
			}
			// Kiem tra file co ton tai khong
			if (fileSystem->fileTable[id] == NULL)
			{
				printf("\nKhong the seek vi file nay khong ton tai.");
				kernel->machine->WriteRegister(2, -1);
				increasePC();
				return;
			}
			// Kiem tra co goi Seek tren console khong
			if (id == 0 || id == 1)
			{
				printf("\nKhong the seek tren file console.");
				kernel->machine->WriteRegister(2, -1);
				increasePC();
				return;
			}
			// Neu pos = -1 thi gan pos = Length nguoc lai thi giu nguyen pos
			pos = (pos == -1) ? fileSystem->fileTable[id]->Length() : pos;
			if (pos > fileSystem->fileTable[id]->Length() || pos < 0) // Kiem tra lai vi tri pos co hop le khong
			{
				printf("\nKhong the seek file den vi tri nay.");
				kernel->machine->WriteRegister(2, -1);
			}
			else
			{
				// Neu hop le thi tra ve vi tri di chuyen thuc su trong file
				fileSystem->fileTable[id]->Seek(pos);
				kernel->machine->WriteRegister(2, pos);
			}
			increasePC();
			return;
		}
    case SC_Read: {
      // Input: buffer(char*), so ky tu(int), id cua file(OpenFileID)
      // Output: -1: Loi, So byte read thuc su: Thanh cong, -2: Thanh cong
      // Cong dung: Doc file voi tham so la buffer, so ky tu cho phep va id cua
      // file
      
      int virAddr;   // chua dia chi chuoi buffer
      int charcount; // chua charcount la so ky tu duoc doc
      int id;        // id cua file
      int curPosition;
      int newPosition;
      char *buffer;
      int readBytes;

      DEBUG(dbgFile, "\n SC_Read call ...");
      DEBUG(dbgFile, "\n Reading virtual address of buffer");
      virAddr = kernel->machine->ReadRegister(4);
      DEBUG(dbgFile, "\n Reading charcount.");
      charcount = kernel->machine->ReadRegister(5);
      DEBUG(dbgFile, "\n Reading id.");
      id = kernel->machine->ReadRegister(6);
      
      // Kiem tra file id co hop le
      if (id < 0 || id > MAX_FILE_OPEN) {
        printf("\nInvalid file id.");
        kernel->machine->WriteRegister(2, RW_ERROR_FILE);
        increasePC();
        return;
      }

      // Kiem tra file co ton tai khong
      if (fileSystem->fileTable[id] == NULL) {
        printf("\nFile does not exist in file table");
        kernel->machine->WriteRegister(2, RW_ERROR_FILE);
        increasePC();
        return;
      }

      // Truong hop doc file stdout
      if (id == INDEX_STDOUT) {
        printf("\nCan't read file stdout.");
        kernel->machine->WriteRegister(2, RW_ERROR_FILE);
        increasePC();
        return;
      }

      // Truong hop file doc duoc
     
      curPosition = fileSystem->fileTable[id]->getCurrentOffset(); // Lay vi tri current position
      buffer = new char[charcount];           //

      // Truong hop doc file stdin (id la 0)
      if (id == INDEX_STDIN) // chua xu ly cham cuoi file -> -2
      {
        // Su dung ham Read cua lop SynchConsole de doc max charcount byte ->
        // bufffer
        // tra ve so byte thuc su doc duoc
        
        int size = kernel->synchConsoleIn->Read(buffer, charcount);
      
        System2User(virAddr, size,
                    buffer); // Copy chuoi tu vung nho System Space (buffer)
                             // sang User Space (virAddr) voi buffer co do dai
                             // la size (so byte thuc su doc duoc)
        kernel->machine->WriteRegister(2,
                                       size); // Tra ve so byte thuc su doc duoc

        delete buffer;
        increasePC();
        return;
      }

      // Truong hop doc file binh thuong
      if ((fileSystem->fileTable[id]->Read(buffer, charcount)) > 0) {
        // So byte thuc su = newPosition - curPosition
        newPosition = fileSystem->fileTable[id]->getCurrentOffset();
        // Copy chuoi tu vung nho System Space sang User Space voi buffer co do
        // dai la readBytes (so byte that su da doc)
        readBytes = newPosition - curPosition;
        System2User(virAddr, readBytes, buffer);

        kernel->machine->WriteRegister(2, readBytes);
      } else // Truong hop con lai: doc va cham toi cuoi file tra ve -2
      {
        kernel->machine->WriteRegister(2, RW_END_FILE);
      }

      delete buffer;
      increasePC();
      return;
    }

    case SC_Write: {
      // Input: buffer(char*), so ky tu(int), id cua file(OpenFileID)
      // Output: -1: Loi, So byte write thuc su: Thanh cong, -2: Thanh cong
      // Cong dung: Ghi file voi tham so la buffer, so ky tu cho phep va id cua
      // file
      int virAddr;   // chua dia chi chuoi buffer
      int charcount; // chua charcount la so ky tu duoc doc
      int id;        // id cua file

      int curPosition;
      int newPosition;
      char *buffer;
      int readBytes;

      DEBUG(dbgFile, "\n SC_Read call ...");
      DEBUG(dbgFile, "\n Reading virtual address of buffer");
      virAddr = kernel->machine->ReadRegister(4);
      DEBUG(dbgFile, "\n Reading charcount.");
      charcount = kernel->machine->ReadRegister(5);
      DEBUG(dbgFile, "\n Reading id.");
      id = kernel->machine->ReadRegister(6);

      // Kiem tra file id co hop le
      if (id < 0 || id > MAX_FILE_OPEN) {
        printf("\nInvalid file id.");
        kernel->machine->WriteRegister(2, RW_ERROR_FILE);
        increasePC();
        return;
      }

      // Kiem tra file co ton tai khong
      if (fileSystem->fileTable[id] == NULL) {
        printf("\nFile does not exist in file table");
        kernel->machine->WriteRegister(2, RW_ERROR_FILE);
        increasePC();
        return;
      }

      // Xet truong hop ghi vao file only read hoac file stdin thi tra ve
      // RW_ERROR_FILE
      if (fileSystem->fileTable[id]->getType() == READONLY_TYPE ||
          id == INDEX_STDIN) {
        printf("\nCan't write in file stdin or file only read.");
        kernel->machine->WriteRegister(2, RW_ERROR_FILE);
        increasePC();
        return;
      }

      curPosition = fileSystem->fileTable[id]
                        ->getCurrentOffset(); // Kiem tra thanh cong thi lay vi
                                              // tri curPosition
      buffer = User2System(virAddr,
                           charcount); // Copy vung nho User Space sang System
                                       // Space voi buffer dai charcount bytes
      if (id == INDEX_STDOUT)          // Xet truong hop file stdout
      {
        int pos = 0;
        // Ghi ma cuoi file la sao ta?
        while (buffer[pos] != '\0') {

          // write moi byte trong file ra sdt out
          // gSynchConsole->Write(buffer + pos, 1); // Su dung ham Write cua lop
          // SynchConsole
          kernel->synchConsoleOut->PutChar((buffer + pos)[0]);
          // den byte tiep theo
          pos++;
        }

        kernel->machine->WriteRegister(
            2, pos - 1); // Tra ve so byte thuc su write duoc
        delete buffer;
        increasePC();
        return;
      }
      // Xet truong hop ghi file read & write thi tra ve so byte thuc su
      if (fileSystem->fileTable[id]->getType() == READWRITE_TYPE) {
        if ((fileSystem->fileTable[id]->Write(buffer, charcount)) > 0) {
          // So byte thuc su = newPosition - curPosition
          newPosition = fileSystem->fileTable[id]->getCurrentOffset();
          kernel->machine->WriteRegister(2, newPosition - curPosition);
          delete buffer;
          increasePC();
          return;
        } else {
          kernel->machine->WriteRegister(2, RW_END_FILE);
        }
      }
    }
    case SC_Join:
      // input: SpaceID id
      // output: exit code cho tien trinh da dang block, err: -1
      // purpose: doi va block dua tren id
    {
      int pID, result;
      pID = kernel->machine->ReadRegister(4); // doc SpaceID id tu r4
      result = pTab->JoinUpdate(pID); // join vao tien trinh cha
      // tra ve ket qua thuc hien
      kernel->machine->WriteRegister(2, result);
      increasePC();
      return;
    }
    case SC_Exec:
    {
      int virtAddr;
      char* name;
      OpenFile *oFile;
      int id;
			virtAddr = kernel->machine->ReadRegister(4);	// doc dia chi ten chuong trinh tu thanh ghi r4
			
			name = User2System(virtAddr, MAX_LENGTH_FILENAME + 1); // Lay ten chuong trinh, nap vao kernel
	
			if(name == NULL)
			{
				DEBUG('a', "\n Not enough memory in System");
				printf("\n Not enough memory in System");
				kernel->machine->WriteRegister(2, -1);
				increasePC();
				return;
			}
			oFile = fileSystem->Open(name);
			if (oFile == NULL)
			{
				printf("\nExec:: Can't open this file.");
				kernel->machine->WriteRegister(2,-1);
				increasePC();
				return;
			}

			delete oFile;

			// Return child process id
			id = pTab->ExecUpdate(name); 
			kernel->machine->WriteRegister(2,id);

			delete[] name;	
			increasePC();
			return;

    }
    
    

    case SC_Exit: {
      // input: exit code
      // output: exit code cho tien trinh da join. Thanh cong: 0, err: exit code
      int exitStatus,result;
      exitStatus = kernel->machine->ReadRegister(4);
      result = pTab->ExitUpdate(exitStatus);
      //kernel->machine->WriteRegister(2, result);

      increasePC();
      return;
    }
    case SC_CreateSemaphore: {
      // int CreateSemaphore(char* name, int semval).
      int virtAddr = kernel->machine->ReadRegister(4);
      int semval = kernel->machine->ReadRegister(5);

      char *name = User2System(virtAddr, MAX_LENGTH_FILENAME + 1);
      if (name == NULL) {
        DEBUG('a', "\n Not enough memory in System");
        printf("\n Not enough memory in System");
        kernel->machine->WriteRegister(2, -1);
        delete name;
        increasePC();
        return;
      }

      int res = sTab->Create(name, semval);

      if (res == -1) {
        DEBUG('a', "\n Khong the khoi tao semaphore");
        printf("\n Khong the khoi tao semaphore");
        kernel->machine->WriteRegister(2, -1);
        delete name;
        increasePC();
        return;
      }

      delete name;
      kernel->machine->WriteRegister(2, res);
      increasePC();
      return;
    }

    case SC_Wait: {
      // int Wait(char* name)
      int virtAddr = kernel->machine->ReadRegister(4);

      char *name = User2System(virtAddr, MAX_LENGTH_FILENAME + 1);
      if (name == NULL) {
        DEBUG('a', "\n Not enough memory in System");
        printf("\n Not enough memory in System");
        kernel->machine->WriteRegister(2, -1);
        delete name;
        increasePC();
        return;
      }

      int res = sTab->Wait(name);

      if (res == -1) {
        DEBUG('a', "\n Khong ton tai ten semaphore nay!");
        printf("\n Khong ton tai ten semaphore nay!");
        kernel->machine->WriteRegister(2, -1);
        delete name;
        increasePC();
        return;
      }

      delete name;
      kernel->machine->WriteRegister(2, res);
      increasePC();
      return;
    }
    case SC_Signal: {
      // int Signal(char* name)
      int virtAddr = kernel->machine->ReadRegister(4);

      char *name = User2System(virtAddr, MAX_LENGTH_FILENAME + 1);
      if (name == NULL) {
        DEBUG('a', "\n Not enough memory in System");
        printf("\n Not enough memory in System");
        kernel->machine->WriteRegister(2, -1);
        delete name;
        increasePC();
        return;
      }

      int res = sTab->Signal(name);

      if (res == -1) {
        DEBUG('a', "\n Khong ton tai ten semaphore nay!");
        printf("\n Khong ton tai ten semaphore nay!");
        kernel->machine->WriteRegister(2, -1);
        delete name;
        increasePC();
        return;
      }

      delete name;
      kernel->machine->WriteRegister(2, res);
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
