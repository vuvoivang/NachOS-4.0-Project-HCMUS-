#include "ptable.h"
#include "main.h"

PTable::PTable(int size)
{
  // khoi ptable
  int i = 0;
  psize = size;
  // khoi tao bm va bmsem de su dung
  bm = new Bitmap(size);
  bmsem = new Semaphore("Bitmapsem", 1);
  // khoi tao NULL cho moi con tro PCB
  for (i = 0; i < MAXPROCESS; i++)
    pcb[i] = NULL;

  bm->Mark(0);

  // for main process
  pcb[0] = new PCB(0);
  pcb[0]->parentID = -1;
}

// set file nam for main thread
void PTable::SetFileNameMainThread(char *filename) {
  pcb[0]->SetFileName(filename);
}

PTable::~PTable()
{

  int i = 0;
  if (bm != NULL)
    delete bm;

  for (i = 0; i < MAXPROCESS; i++)
    if (pcb[i] != NULL)
      delete pcb[i];

  if (bmsem != NULL)
    delete bmsem;
}

//--------------------------------------------------------------------

int PTable::ExecUpdate(char *filename) {
  // Khong cho phep nap 2 tien trinh 1 luc
  bmsem->P();

  if (filename == NULL) {
    printf("\nCan't not execute name is NULL.\n");
    bmsem->V();
    return -1;
  }

  // Kiem tra chuong trinh duoc goi co la chinh no hay khong
  if (strcmp(filename, kernel->currentThread->getName()) == 0) {
    printf("\nKhong duoc phep goi chinh no !!!\n");
    bmsem->V();
    return -1;
  }

  // lay fileSystem cua current thread de mo file
  FileSystem* fileSystem = pTab->getFileTable(kernel->currentThread->processID);
  OpenFile *fileOpen = fileSystem->Open(filename);
  // Kiem tra mo file
  // Khong mo duoc
  if (fileOpen == NULL) {
    printf("\nKhong the mo file nay %s\n", filename);
    bmsem->V();
    return -1;
  }

  // Kiem tra con slot trong khong de luu tien trinh hay khong
  int idSlot = GetFreeSlot();

  if (idSlot == -1) {
    printf("\nKhong con slot trong !!!\n");
    bmsem->V();
    return -1;
  }

  // Tao moi instance PCB de quan ly process
  pcb[idSlot] = new PCB(idSlot);
  pcb[idSlot]->SetFileName(filename);

  // Gan parent ID tuong ung
  pcb[idSlot]->parentID = kernel->currentThread->processID;

  bm->Mark(idSlot);

  int processID = pcb[idSlot]->Exec(filename, idSlot);

  delete fileOpen;
  bmsem->V();
  return processID;
}

int PTable::ExitUpdate(int exitCode) {

  // Kiem tra pID co ton tai khong
  int processID = kernel->currentThread->processID;


  // Main process: goi Halt
  if (processID == 0)
  {
    kernel->currentThread->FreeSpace();
    kernel->interrupt->Halt();
    return 0;
  }

  if (!IsExist(processID))
  {
    printf("\nTien trinh khong ton tai !!!\n");
    return -1;
  }

  pcb[processID]->SetExitCode(exitCode);
  pcb[pcb[processID]->parentID]->DecNumWait();

  // JoinRelease va ExitWait de giai phong su cho doi cho tien trinh cha va xin
  // phep ket thuc 
  pcb[processID]->JoinRelease();
  // Xin phep tien trinh cha cho ket thuc
  pcb[processID]->ExitWait();
  Remove(processID);
  kernel->currentThread->FreeSpace();
  // kernel->currentThread->Finish();
  return exitCode;
}

int PTable::JoinUpdate(int pID)
{

  if (!IsExist(pID))
  {
    printf("Khong ton tai process id nay! %d\n", pID);
    return -1;
  }

  // Kiem tra tien trinh join vao chinh no
  if (kernel->currentThread->processID == pID)
  {
    printf("\nTien trinh khong duoc join vao chinh no !!!\n");
    return -1;
  }

  // Khi tien trinh join hop le
  // Tien trinh hien tai co la cha cua tien trinh join
  if (kernel->currentThread->processID != pcb[pID]->parentID)
  {
    printf(
        "\nKo duoc phep join vao tien trinh khong phai cha cua no !!!\n");
    return -1;
  }
  // Tang numwait
  pcb[pcb[pID]->parentID]->IncNumWait();
  
  pcb[pID]->JoinWait(); // Tien trinh cha cho doi cho den khi tien trinh con ket thuc
                        

  int exitCode = pcb[pID]->GetExitCode();

  pcb[pID]->ExitRelease(); // cho phep tien trinh con ket thuc

  // tra ve exit code
  return exitCode;
}

int PTable::GetFreeSlot()
{
  // Tim slot de luu thong tin cho tien trinh moi
  return bm->FindAndSet();
}

bool PTable::IsExist(int pID)
{
  if (pID < 0 || pID >= MAXPROCESS)
    return 0;
  return bm->Test(pID);
}

void PTable::Remove(int pID)
{
  // Xoa process co id la pID ra khoi PTable
  if (pID < 0 || pID >= MAXPROCESS)
    return;
  if (bm->Test(pID))
  {
    bm->Clear(pID);
    delete pcb[pID];
  }
}
char *PTable::GetFileName(int id) { return (pcb[id]->GetFileName()); }

FileSystem *PTable::getFileTable(int id)
{
  return pcb[id]->fileTable;
}
