#include "pcb.h"
#include "utility.h"
#include "main.h"
#include "thread.h"
#include "addrspace.h"

PCB::PCB(int id)
{
	joinsem= new Semaphore("JoinSem",0);
	exitsem= new Semaphore("ExitSem",0);
	mutex= new Semaphore("Mutex",1);
	pid= id;
	exitcode= 0;
	numwait= 0;
	if(id)
		parentID= kernel->currentThread->processID;
	else {
		parentID= 0;
		this->SetFileName("./test/scheduler");
	}
	thread= NULL;
	JoinStatus= -1;

}

PCB::~PCB()
{
	if(joinsem != NULL)
		delete joinsem;
	if(exitsem != NULL)
		delete exitsem;
	if(mutex != NULL)
		delete mutex;
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
	JoinStatus= parentID;
	IncNumWait();
	joinsem->P();
}

void PCB::JoinRelease()
{
	DecNumWait();
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

//------------------------------------------------------------------
int PCB::Exec(char *filename, int pID)
{
	mutex->P();
	thread= new Thread(filename);
	if(thread == NULL)
	{
		printf("\nLoi: Khong tao duoc tien trinh moi !!!\n");
		mutex->V();
		return -1;
	}
	thread->processID= pID;
	thread->Fork((VoidFunctionPtr)StartProcess_2,(void*)pID); // phan than ra then thu 2 de exec
	mutex->V();
	return pID;
}

//*************************************************************************************


void StartProcess_2(int id) // fork con tro ham den bo nho cua tien trinh con
{
    char* fileName = pTab->GetName(id);

    AddrSpace *space;
    space = new AddrSpace(fileName);

	if(space == NULL)
	{
		printf("\nPCB::Exec : Can't create AddSpace.");
		return;
	}

    kernel->currentThread->space = space;

    space->InitRegisters();		
    space->RestoreState();		

    kernel->machine->Run();		
    ASSERT(FALSE);		
}

