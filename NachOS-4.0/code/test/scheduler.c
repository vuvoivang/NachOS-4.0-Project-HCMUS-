#include "syscall.h"

int main()
{
	int pingID, pongID;
	PrintString("Ping-Pong test starting ...\n\n");
	pingID = Exec("../test/ping");
	pongID = Exec("../test/pong");
	//PrintNum(pingID);
	//PrintNum(pongID);
	Join(pingID);
	Join(pongID);
	// PrintNum(pongID);
	// PrintString("\n\n");
}