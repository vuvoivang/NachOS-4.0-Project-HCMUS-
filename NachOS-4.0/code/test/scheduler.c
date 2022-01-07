#include "syscall.h"

int main()
{
	int pingID, pongID,codeJoinPing, codeJoinPong;
	//PrintString("Ping-Pong test starting ...\n\n");

	pingID = Exec("../test/ping");
	pongID = Exec("../test/pong");

	// PrintString("\nProcess space Id: \n");
	// PrintNum(pingID);
	// PrintNum(pongID);

	// PrintString("\nJoin ping: \n");
	codeJoinPing = Join(pingID);

	// PrintNum(codeJoinPing);
	// PrintString("\n");

	codeJoinPong = Join(pongID);

	// PrintNum(codeJoinPong);
	// PrintString("\n");

	// PrintString("\n\n");
	// PrintString("end scheduler");
}