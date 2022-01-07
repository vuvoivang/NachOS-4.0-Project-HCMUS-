// filesys.h
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system.
//	The "STUB" version just re-defines the Nachos file system
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.
//
//	The other version is a "real" file system, built on top of
//	a disk simulator.  The disk is simulated using the native UNIX
//	file system (in a file named "DISK").
//
//	In the "real" implementation, there are two key data structures used
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "openfile.h"
#include "sysdep.h"

#define MAX_FILE_OPEN 10
#define INPUT_TYPE 1 
#define OUTPUT_TYPE 0
#define READONLY_TYPE 3
#define READWRITE_TYPE 2
#define INDEX_STDIN 1  
#define INDEX_STDOUT 0 
#ifdef FILESYS_STUB
// #ifdef FILESYS_STUB // Temporarily implement file system calls as
// calls to UNIX, until the real file system
// implementation is available
class FileSystem {
public:
  OpenFile **fileTable; // quan ly file dang open

  // define ham constructor
  FileSystem() {
    fileTable = new OpenFile *[MAX_FILE_OPEN];
    for (int i = 0; i < MAX_FILE_OPEN; i++) {
      fileTable[i] = NULL;
    }
    // luon luon ton tai
    this->Create("stdin");
    this->Create("stdout");

    fileTable[INDEX_STDIN] = this->Open("stdin", INPUT_TYPE);
    fileTable[INDEX_STDOUT] = this->Open("stdout", OUTPUT_TYPE);
  }
  // define destructor
  ~FileSystem() {
    for (int i = 0; i < MAX_FILE_OPEN; ++i) {
      if (fileTable[i] != NULL)
        delete fileTable[i];
    }
    delete[] fileTable;
  }

  // default method
  bool Create(char *name) {
    int fileDescriptor = OpenForWrite(name);

    if (fileDescriptor == -1)
      return FALSE;
    Close(fileDescriptor);
    return TRUE;
  }

  OpenFile *Open(char *name) {
    int fileDescriptor = OpenForReadWrite(name, FALSE);
    if (fileDescriptor == -1)
      return NULL;
    return new OpenFile(fileDescriptor);
  }

  // overload theo type
  OpenFile *Open(char *name, int type) {
    int fileDescriptor;

    if (type == OUTPUT_TYPE) // user ghi-write  kernel -> doc du lieu nay->read
      fileDescriptor = OpenForWrite(name);
    else if (type ==  INPUT_TYPE|| type == READONLY_TYPE)
      fileDescriptor = OpenForRead(name, FALSE); // mo file ra va chi doc
    else if (type == READWRITE_TYPE)
      fileDescriptor = OpenForReadWrite(name, FALSE); // mo file ra doc hoac ghi

    if (fileDescriptor == -1) // that bai thi tra ve NULL
      return NULL;
    return new OpenFile(fileDescriptor,type);
  }

  // tim slot trong
  int FindFreeSlot() {
    for (int i = 2; i < MAX_FILE_OPEN; i++)
      if (fileTable[i] == NULL)
        return i;

    return -1;
  }
  // default method
  bool Remove(char *name) { return Unlink(name) == 0; }
};

#else // FILESYS
class FileSystem {
public:
  FileSystem(bool format); // Initialize the file system.
                           // Must be called *after* "synchDisk"
                           // has been initialized.
                           // If "format", there is nothing on
                           // the disk, so initialize the directory
                           // and the bitmap of free blocks.

  bool Create(char *name, int initialSize);
  // Create a file (UNIX creat)

  OpenFile *Open(char *name); // Open a file (UNIX open)

  bool Remove(char *name); // Delete a file (UNIX unlink)

  void List(); // List all the files in the file system

  void Print(); // List all the files and their contents

private:
  OpenFile *freeMapFile;   // Bit map of free disk blocks,
                           // represented as a file
  OpenFile *directoryFile; // "Root" directory -- list of
                           // file names, represented as a file
};

#endif // FILESYS

#endif // FS_H
