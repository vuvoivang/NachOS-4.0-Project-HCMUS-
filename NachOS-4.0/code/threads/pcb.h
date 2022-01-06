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
	int		numwait;	//so tien trinh da join
	
	Thread		*thread;

	char     	FileName[32]; // ten tien trinh

public:
	int 		parentID;	//ID cua tien trinh cha

	PCB(int id);
	~PCB();

	int Exec(char *filename, int pID); //nap chuong trinh co ten luu trong bien filename va processID se la pID
	int GetID();
	int GetNumWait();

	void JoinWait(); // 1. Tiến trình cha đợi tiến trình con kết thúc
	void ExitWait(); // Tiến trình con kết thúc
	
	void JoinRelease(); // Báo cho tiến trình cha thực thi tiếp
	void ExitRelease(); // Cho phép tiến trình con kết thúc

	void IncNumWait(); // Tăng số tiến trình chờ
	void DecNumWait(); // Giảm số tiến trình chờ

	void SetExitCode(int ec); // Đặt exitcode của tiến trình
	int GetExitCode(); // Trả về exitcode

	void SetFileName(char* fn); // Set ten tien trinh
	char* GetFileName();  // Tra ve ten tien trinh

};

void StartProcess_2(int id);

#endif // PCB_H
