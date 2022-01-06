#ifndef PCB_H
#define PCB_H

#include "thread.h"
#include "synch.h"

// Process Control Block
class PCB
{
private:
	Semaphore	*joinsem;	//semaphore cho qua trinh join
	Semaphore	*exitsem;	//semaphore cho qua trinh exit
	Semaphore	*multex;
	int		exitcode;
	Thread		*thread;
	int		pid; //id cua tien trinh
	int		numwait;	//so tien trinh da join
	// bo sung them ten tien trinh 
	char     	FileName[32];
public:
	int 		parentID;	//ID cua tien trinh cha
		//Trang thai co Join voi tien trinh nao khong? neu co thi gia tri chinh la ID cua tien trinh ma no Join

	PCB(int id);
	~PCB();
	
	int Exec(char *filename, int pID); //nap chuong trinh co ten luu trong bien filename va processID se la pID
	int GetID();
	int GetNumWait();
	void JoinWait(); //down joinSem
	void ExitWait();//down exitSem
	void JoinRelease();//up joinSem
	void ExitRelease();//up exitSem
	void IncNumWait();
	void DecNumWait();
	void SetExitCode(int ec);
	int GetExitCode();
	void SetFileName(char* fn);
	char* GetFileName();

};

void StartProcess_2(int id);
#endif // PCB_H
