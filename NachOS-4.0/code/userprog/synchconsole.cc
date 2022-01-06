// synchconsole.cc 
//	Routines providing synchronized access to the keyboard 
//	and console display hardware devices.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synchconsole.h"

//----------------------------------------------------------------------
// SynchConsoleInput::SynchConsoleInput
//      Initialize synchronized access to the keyboard
//
//      "inputFile" -- if NULL, use stdin as console device
//              otherwise, read from this file
//----------------------------------------------------------------------

SynchConsoleInput::SynchConsoleInput(char *inputFile)
{
    consoleInput = new ConsoleInput(inputFile, this);
    lock = new Lock("console in");
    waitFor = new Semaphore("console in", 0);
    RLineBlock = new Semaphore("Read Synch Line Block",1);
	
}

//----------------------------------------------------------------------
// SynchConsoleInput::~SynchConsoleInput
//      Deallocate data structures for synchronized access to the keyboard
//----------------------------------------------------------------------

SynchConsoleInput::~SynchConsoleInput()
{ 
    delete consoleInput; 
    delete lock; 
    delete waitFor;
   
    delete RLineBlock;
}

//----------------------------------------------------------------------
// SynchConsoleInput::GetChar
//      Read a character typed at the keyboard, waiting if necessary.
//----------------------------------------------------------------------

char
SynchConsoleInput::GetChar()
{
    char ch;

    lock->Acquire();
    waitFor->P();	// wait for EOF or a char to be available.
    ch = consoleInput->GetChar();
    lock->Release();
    return ch;
}

//----------------------------------------------------------------------
// SynchConsoleInput::CallBack
//      Interrupt handler called when keystroke is hit; wake up
//	anyone waiting.
//----------------------------------------------------------------------

void
SynchConsoleInput::CallBack()
{
    waitFor->V();
}
int
SynchConsoleInput::Read(char *into, int numBytes)
{
	int loop;
	int eolncond = FALSE;
	char ch;

	for (loop = 0; loop < numBytes; loop++)
		into[loop] = 0;

	loop = 0;

	RLineBlock->P();				// Block for a read line

//	printf("{%s}:\n",currentThread->getName());	// DEBUG print thread

	while ( (loop < numBytes) && (eolncond == FALSE) )
	{
		do
		{
			waitFor->P();		// Block for single char
			ch = consoleInput->GetChar();		// Get a char (could)
		} while ( ch == EOF);
		
		if ( (ch == '\012') || (ch == '\001') )
		{
			eolncond = TRUE;
		}
		else
		{
			into[loop] = ch;		// Put the char in buf
			loop++;				// Auto inc
		}
	}

	RLineBlock->V();				// UnBLock

	if (ch == '\001')				// CTRL-A Returns -1
		return -1;				// For end of stream
	else
		return loop;				// How many did we rd
}

//----------------------------------------------------------------------
// SynchConsoleOutput::SynchConsoleOutput
//      Initialize synchronized access to the console display
//
//      "outputFile" -- if NULL, use stdout as console device
//              otherwise, read from this file
//----------------------------------------------------------------------

SynchConsoleOutput::SynchConsoleOutput(char *outputFile)
{
    consoleOutput = new ConsoleOutput(outputFile, this);
    lock = new Lock("console out");
    waitFor = new Semaphore("console out", 0);
	WLineBlock = new Semaphore("Write Synch Line Block",1);
    
}

//----------------------------------------------------------------------
// SynchConsoleOutput::~SynchConsoleOutput
//      Deallocate data structures for synchronized access to the keyboard
//----------------------------------------------------------------------

SynchConsoleOutput::~SynchConsoleOutput()
{ 
    delete consoleOutput; 
    delete lock; 
    delete waitFor;
	delete WLineBlock;
}

//----------------------------------------------------------------------
// SynchConsoleOutput::PutChar
//      Write a character to the console display, waiting if necessary.
//----------------------------------------------------------------------

void
SynchConsoleOutput::PutChar(char ch)
{
    lock->Acquire();
    consoleOutput->PutChar(ch);
    waitFor->P();
    lock->Release();
}

//----------------------------------------------------------------------
// SynchConsoleOutput::CallBack
//      Interrupt handler called when it's safe to send the next 
//	character can be sent to the display.
//----------------------------------------------------------------------

void
SynchConsoleOutput::CallBack()
{
    waitFor->V();
}
int SynchConsoleOutput::Write(char *from, int numBytes)
{
	int loop;			// General purpose counter

	WLineBlock->P();			// Block for the line

//	printf("[%s]:\n",currentThread->getName());	//DEBUG: Print thread

	for (loop = 0; loop < numBytes; loop++)
	{
		consoleOutput->PutChar(from[loop]);		// Write and wait
		waitFor->P();			// Block for a character
	}

	WLineBlock->V();				// Free Up
	return numBytes;				// Return the bytes out
}
