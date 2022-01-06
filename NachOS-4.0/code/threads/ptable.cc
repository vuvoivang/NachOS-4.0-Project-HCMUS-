#include "ptable.h"
#include "main.h"


PTable::PTable(int size)
{
	int i = 0;
	if(size<0)
		return;
	psize = size;
	bm = new Bitmap(size);  // size=10
	bmsem = new Semaphore("bmsem",1); // dung de thuc thi
		
	for(i = 0 ; i < MAXPROCESS ; ++i)
		pcb[i] = NULL; // chua co pcb nao duoc thuc thi
	bm->Mark(0); // danh dau tat ca mang bitmap =0
}

PTable::~PTable()
{
	int i=0;
	if(bm!=NULL)
		delete bm;
	if(bmsem!=NULL)
		delete bmsem;
	for(i=0; i<MAXPROCESS; i++)
		if(pcb[i]!=NULL)
			delete pcb[i];
}

//--------------------------------------------------------------------

// tra ve ID cua Tien trinh
int PTable::ExecUpdate(char* filename)
{
	bmsem->P();			//chi nap 1 tien trinh vao mot thoi diem

//Kiem tra file co ton tai tren may khong
	int pID;
	int ID;
	OpenFile *executable ;
	executable = fileSystem->Open(filename);
	if (executable == NULL) 
	{
		printf("\n!!!Unable to open file %s\n", filename);
		bmsem->V(); // up semaphore
		return -1;
    }
	delete executable;			// close file
////////////////////////////////////////////////////////////

//Kiem tra chuong trinh duoc goi co la chinh no khong
	if(!strcmp(filename,kernel->currentThread->getName()))
	{
		printf("\nLoi: khong duoc phep goi exce chinh no !!!\n");
		bmsem->V();
		return -1;
	}
////////////////////////////////////////////////////////////

//Kiem tra con slot trong khong
	ID= GetFreeSlot();
	if(ID==-1)
	{
		printf("\nLoi: Da vuot qua 10 tien trinh !!!\n");
		bmsem->V();
		return -1;
	}
////////////////////////////////////////////////////////////

	pcb[ID]= new PCB(ID); // da tim duoc slot trong
	pcb[ID]->SetFileName(filename);
	bm->Mark(ID); // danh dau da su dung slot nay
	pID= pcb[ID]->Exec(filename,ID);// return ve ID

	bmsem->V();
	return pID;
}

// input la 1 exitCode kieu integer
// ket thuc 1 tien trinh dang chay (currentThread)
// tra ve exit code
int PTable::ExitUpdate(int exitCode)
{
//Kiem tra pID co ton tai khong
	int pID;
	pID= kernel->currentThread->processID;
	if(!IsExist(pID))
	{
		printf("\nLoi: Tien trinh khong ton tai !!!\n");
		return -1;
	}
//////////////////////////////////////////////////////////////

//Neu la main process thi Halt()
	if(pID==0)
	{
		kernel->currentThread->FreeSpace();
		kernel->interrupt->Halt();
		return 0;
	}
	pcb[pcb[pID]->parentID]->DecNumWait();
/////////////////////////////////////////////////////////////
	// pcb[pcb[pID]->parentID]->DecNumWait();// giam numwait cha di 1
	pcb[pID]->SetExitCode(exitCode);
	
	cout<<"\nExit code cua "<<pID<<" la   "<<exitCode<<"\n\n";
	
	pcb[pID]->JoinRelease();
	pcb[pID]->ExitWait();
	Remove(pID);	
	
	
	return exitCode;
}
int PTable::JoinUpdate(int pID)
{
	int ec;
	if(pID <= 0 || pID > 9)
	{
		printf("\nLoi: Khong ton tai process: id = %d\n",pID);
		return -1;
	}

	if (pcb[pID] == NULL)
	{
		printf("Loi: Khong ton tai process id nay!");
		return -1;
	}

//kiem tra tien trinh dang chay co la cha cua tien trinh can join hay khong
	if(kernel->currentThread->processID != pcb[pID]->parentID)
	{
		printf("\nLoi: Ko duoc phep join vao tien trinh khong phai cha cua no !!!\n");
		return -1;
	}
/////////////////////////////////////////////////////////////////////////////////////////////
	
	
	pcb[pID]->JoinWait(); 	//doi den khi tien trinh con ket thuc
	pcb[pcb[pID]->parentID]->IncNumWait();
	// pcb[pcb[pID]->parentID]->IncNumWait();
	ec = pcb[pID]->GetExitCode();
	cout<<"\n\nExit code la "<<ec<<"\n\n";
	if(ec != 0)
	{
		printf("\nProcess exit with exitcode EC = %d ",ec);
		return -1;
	}

	pcb[pID]->ExitRelease();	//cho phep tien trinh con ket thuc
	// tra ve exit code
	return 0;
}

void PTable::Remove(int pID)
{
	if(pID<0 || pID>9)
		return;
	if(bm->Test(pID))
	{
		bm->Clear(pID);
		delete pcb[pID];
	}
}

//----------------------------------------------------------------------------------------------
int PTable::GetFreeSlot()
{
	return bm->FindAndSet();
}

bool PTable::IsExist(int pID)
{
	if(pID<0 || pID>9)
		return 0;
	return bm->Test(pID);
}

char* PTable::GetName(int pID)
{
	if(pID>=0 && pID<10 && bm->Test(pID))
		return pcb[pID]->GetFileName();
}