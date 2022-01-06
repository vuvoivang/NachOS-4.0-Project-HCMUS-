#include "pcb.h"
#include "utility.h"
#include "main.h"
#include "thread.h"
#include "addrspace.h"

PCB::PCB(int id)
{
	joinsem= new Semaphore("joinSem",0);
	exitsem= new Semaphore("exitSem",0);
	multex= new Semaphore("multex",1);
	pid= id;
	exitcode= 0;
	numwait= 0;
	if(id)
		parentID= kernel->currentThread->processID;
	else {
		parentID= 0;
		
	}
	thread= NULL;
	

}

PCB::~PCB()
{
	if(joinsem != NULL)
		delete joinsem;
	if(exitsem != NULL)
		delete exitsem;
	if(multex != NULL)
		delete multex;
}

//------------------------------------------------------------------
int PCB::GetID()
{
	return pid;
}

int PCB::GetNumWait()
{
	return numwait;
}

int PCB::GetExitCode()
{
	return exitcode;	
}

void PCB::SetExitCode(int ec)
{
	exitcode= ec;
}

void PCB::IncNumWait()
{
	numwait++;
}

void PCB::DecNumWait()
{
	if(numwait)
		numwait--;
}

void PCB::SetFileName(char* fn)
{ 
	strcpy(FileName,fn);
}

char* PCB::GetFileName() 
{ 
	return this->FileName; 
}

//-------------------------------------------------------------------
void PCB::JoinWait()
{
	
	
	joinsem->P();
}

void PCB::JoinRelease()
{
	
	joinsem->V();
}

void PCB::ExitWait()
{
	exitsem->P();
}

void PCB::ExitRelease()
{
	exitsem->V();
}

// truyen vao filename thuc thi va id cua process de cap nhat cho thread
//------------------------------------------------------------------
int PCB::Exec(char *filename, int pID)
{
	multex->P();
	thread= new Thread(filename);
	if(thread == NULL)
	{
		printf("\nLoi: Khong tao duoc tien trinh moi !!!\n");
		multex->V();
		return -1;
	}
	thread->processID= pID; //update process cua thread
	thread->Fork((VoidFunctionPtr)StartProcess_2,(void*)pID); // phan than ra then thu 2 de exec
	multex->V();
	return pID;
}

//*************************************************************************************


//id cua process trong pTab
void StartProcess_2(int id) // fork con tro ham den bo nho cua tien trinh con
{
	AddrSpace *space;
    char* fileName;
	fileName = pTab->GetName(id);
    space = new AddrSpace(fileName); // khoi tao space cho thread thuc thi file nay

	if(space == NULL)
	{
		printf("\nPCB::Exec : Can't create AddSpace.");
		return;
	}

	// exec nen thuc thi ngay chinh "Phan Than cua thread"
    kernel->currentThread->space = space;

    space->InitRegisters();		
    space->RestoreState();		

	cout<<"\nchay vo StartProcess_2 thanh cong";
    kernel->machine->Run();		
    ASSERT(FALSE);		
}