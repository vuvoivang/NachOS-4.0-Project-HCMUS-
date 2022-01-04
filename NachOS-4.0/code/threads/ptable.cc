#include "ptable.h"



PTable::PTable(int size)
{
	int i = 0;
	psize = size;
	bm = new Bitmap(size);
	bmsem = new Semaphore("Bitmap sem",1);
		
	for(i = 0 ; i < MAXPROCESS ; i++)
		pcb[i] = NULL;

	bm->Mark(0);// chua duoc su dung
}

PTable::~PTable()
{

	int i=0;
	if(bm!=NULL)
		delete bm;
	
	for(i=0; i<MAXPROCESS; i++)
		if(pcb[i]!=NULL)
			delete pcb[i];

    if(bmsem!=NULL)
		delete bmsem;
}

//--------------------------------------------------------------------

int PTable::ExecUpdate(char* filename)
{
    // khong cho phep nap 2 tien trinh 1 luc
	bmsem->P();		

	// filename khong ton tai
    if(filename==NULL) {
        bmsem->V();
        return -1;
    }

    //Kiem tra mo file
	OpenFile *fileOpen = fileSystem->Open(filename);
	if (fileOpen == NULL) 
	{
		printf("\nCan't open file %s\n", filename);
		bmsem->V();
		return -1;
    	}
    

    // Kiem tra chuong trinh duoc goi co la chinh no hay khong
	if(strcmp(filename,kernel->currentThread->getName()) == 0)
	{
		printf("\nKhong duoc phep goi chinh no !!!\n");
		bmsem->V();
		return -1;
	}


    //Kiem tra con slot trong khong de luu tien trinh hay khong
	int idSlot = GetFreeSlot();
	//idSlot trong bitmap
	if(idSlot == -1)
	{
		printf("\nKhong con slot trong !!!\n");
		bmsem->V();
		return -1;
	}
    
    // Tao moi instance PCB de quan ly process
	pcb[idSlot]= new PCB(idSlot);
	pcb[idSlot]->SetFileName(filename);

    // Gan parent ID tuong ung
    pcb[idSlot]->parentID = kernel->currentThread->processID;

	bm->Mark(idSlot);

	int processID = pcb[idSlot]->Exec(filename,idSlot);// tra ve idSlot

    delete fileOpen;
	bmsem->V();
	return processID;
}

int PTable::ExitUpdate(int ec)
{
    //Kiem tra pID co ton tai khong

	int processID = currentThread->processID;

    // Doi 1 chut so voi code, theo pdf

    // Main process: goi Halt
    currentThread->FreeSpace();
    if(processID == 0)
	{
		interrupt->Halt();
		return 0;
	}
    currentThread->Finish();

	if(!IsExist(processID))
	{
		printf("\nTien trinh khong ton tai !!!\n");
		return -1;
	}

    
	pcb[processID]->SetExitCode(ec);
	
    // JoinRelease va ExitWait de giai phong su cho doi cho tien trinh cha va xin phep ket thuc
	if(pcb[processID]->JoinStatus != -1)
	{
		pcb[processID]->JoinRelease();
		pcb[processID]->ExitWait();
		Remove(processID);	
	}
	else
		Remove(processID);

	return ec;
}

int PTable::JoinUpdate(int pID)
{
	// if(pID <= 0 || pID > MAXPROCESS)
	// {
	// 	printf("\nKhong ton tai process: id = %d\n",pID);
	// 	return -1;
	// }

	// if (pcb[pID] == NULL)
	// {
	// 	printf("Khong ton tai process id nay!");
	// 	return -1;
	// }

    if (!IsExist(pID))
	{
		printf("Khong ton tai process id nay!");
		return -1;
	}


    // Kiem tra tien trinh join vao chinh no
    if(currentThread->processID == pID)
	{
		printf("\nTien trinh khong duoc join vao chinh no !!!\n");
		return -1;
	}

    // Khi tien trinh join hop le
    // Tien trinh hien tai co la cha cua tien trinh join 
	if(currentThread->processID != pcb[pID]->parentID)
	{
		printf("\nLoi: Ko duoc phep join vao tien trinh khong phai cha cua no !!!\n");
		return -1;
	}

	pcb[pID]->JoinWait(); 	//Tien trinh cha cho doi cho den khi tien trinh con ket thuc

	int ec = pcb[pID]->GetExitCode(); 

	pcb[pID]->ExitRelease();	//cho phep tien trinh con ket thuc
	
	return 0;
}

int PTable::GetFreeSlot()
{
    // Tim slot de luu thong tin cho tien trinh moi
	return bm->Find();
}

bool PTable::IsExist(int pID)
{
	if(pID<0 || pID>9)
		return 0;
	return bm->Test(pID);
}

void PTable::Remove(int pID)
{
    // Xoa process co id la pID ra khoi PTable
	if(pID<0 || pID>9)
		return;
	if(bm->Test(pID))
	{
		bm->Clear(pID);
		delete pcb[pID];
	}
}

char* PTable::GetName(int pID)
{
	if(pID>=0 && pID<10 && bm->Test(pID))
		return pcb[pID]->GetFileName();
}
