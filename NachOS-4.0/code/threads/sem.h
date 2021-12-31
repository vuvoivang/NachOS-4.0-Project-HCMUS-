#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include "synch.h"
#include "bitmap.h"

#endif

class Sem
{
private:
	char name[50];		// Ten cua semaphore
	Semaphore* sem;		// Tao semaphore de quan ly
public:
	// Khoi tao doi tuong Sem. Gan gia tri ban dau la null
	// Nho khoi tao sem su dung
	Sem(char* na, int i);
	~Sem();
	void wait();
	void signal();
	char* GetName();
	
};

