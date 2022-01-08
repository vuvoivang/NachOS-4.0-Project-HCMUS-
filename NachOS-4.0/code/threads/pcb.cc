#include "pcb.h"
#include "addrspace.h"
#include "main.h"
#include "thread.h"
#include "utility.h"

PCB::PCB(int id) {

  if (id == 0)
    this->parentID = -1;
  else
    this->parentID = kernel->currentThread->processID;

  this->numwait = this->exitcode = 0;
  this->thread = NULL;

  // khoi tao semaphore
  this->joinsem = new Semaphore("joinsem", 0);
  this->exitsem = new Semaphore("exitsem", 0);
  this->multex = new Semaphore("multex", 1);
  // khoi tao fileTale cho moi PCB
  this->fileTable = new FileSystem();
}

PCB::~PCB() {
  if (joinsem != NULL)
    delete joinsem;
  if (exitsem != NULL)
    delete exitsem;
  if (multex != NULL)
    delete multex;
  if (thread != NULL) {
    thread->FreeSpace(); // giai phong vung nho
    thread->Finish();
  }
  if(fileTable != NULL){
      delete fileTable;

  }
}

//------------------------------------------------------------------
int PCB::GetID() { return thread->processID; }

int PCB::GetNumWait() { return numwait; }

int PCB::GetExitCode() { return exitcode; }

void PCB::SetExitCode(int ec) { exitcode = ec; }

void PCB::IncNumWait() {
  multex->P();
  ++numwait;
  multex->V();
}

void PCB::DecNumWait() {

  multex->P();
  if (numwait > 0)
    --numwait;
  multex->V();
}

void PCB::SetFileName(char *fn) { strcpy(FileName, fn); }

char *PCB::GetFileName() { return this->FileName; }

//-------------------------------------------------------------------
// Process tranlation to block
// Wait for JoinRelease to continue exec
void PCB::JoinWait() { joinsem->P(); }

// JoinRelease process calling JoinWait
void PCB::JoinRelease() { joinsem->V(); }

// Let process tranlation to block state
// Waiting for ExitRelease to continue exec
void PCB::ExitWait() { exitsem->P(); }

// Release wating process
void PCB::ExitRelease() { exitsem->V(); }

//------------------------------------------------------------------
int PCB::Exec(char *filename, int pID) {
 

  // tránh tình trạng nạp 2 tiến trình cùng 1 lúc
  multex->P(); // down multex

  thread = new Thread(filename);

  if (thread == NULL) {
    printf("\nLoi: Khong tao duoc tien trinh moi !!!\n");
    multex->V();
    return -1;
  }

  // set process ID
  thread->processID = pID;
  parentID = kernel->currentThread->processID;// cap nhat ID cua tien trinh cha trong pcb

  thread->Fork((VoidFunctionPtr)StartProcess_2,
               (void *)pID); // phan than ra then thu 2 de exec
 
  multex->V(); // up multex len tra ve 
  return pID;
}

//*************************************************************************************
void StartProcess_2(int id) 
{
  
  AddrSpace *space;
  char *fileName = pTab->GetFileName(id);

  space = new AddrSpace(fileName); // khoi tao vung nho cho tien trinh

  if (space == NULL) {
    printf("\nPCB::Exec : Can't create AddSpace.");
    return;
  }

  kernel->currentThread->space = space;

  space->InitRegisters();
  space->RestoreState();

  kernel->machine->Run();// chay tien trinh

  ASSERT(FALSE);
}
