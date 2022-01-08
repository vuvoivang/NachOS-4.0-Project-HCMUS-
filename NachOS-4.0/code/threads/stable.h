#ifndef STABLE_H
#define STABLE_H
#include "synch.h"
#include "bitmap.h"
#define MAX_SEMAPHORE 10



// Lop Sem dung de quan ly semaphore.
class Sem
{
private:
	char name[50];		// Ten cua semaphore
	Semaphore* sem;		// Tao semaphore de quan ly
public:
	// Khoi tao doi tuong Sem. Gan gia tri ban dau la null
	Sem(char* na, int i)
	{
		strcpy(this->name, na);
		sem = new Semaphore(this->name, i);
	}

	~Sem()
	{
		if(sem)
			delete sem;
	}

	void wait()
	{
		sem->P();	// Down(sem)
	}

	void signal()
	{
		sem->V();	// Up(sem)
	}
	
	char* getName()
	{
		return this->name;
	}
};

class STable
{
private:
	Bitmap* bm;	// quan ly slot
	Sem* semTab[MAX_SEMAPHORE];	// quan ly 10 semaphore
public:
	STable();		

	~STable();
	// neu ton tai semaphore "name" thi bao loi, neu chua thi tao Semaphore moi
	int Create(char *name, int init);

	// neu ton tai semaphore "name" thi goi this->P() de thuc thi, nguoc lai bao loi
	int Wait(char *name);

	// neu ton tai semaphore "name" thi goi this->V() de thuc thi, nguoc lai bao loi
	int Signal(char *name);
	
	// tim slot trong
	int FindFreeSlot();
	
};
#endif // STABLE_H