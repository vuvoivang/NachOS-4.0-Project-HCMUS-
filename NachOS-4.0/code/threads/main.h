// main.h 
//	This file defines the Nachos global variables
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef MAIN_H
#define MAIN_H

#include "copyright.h"
#include "debug.h"
#include "kernel.h"

#include "synch.h"
#include "bitmap.h"
#include "filesys.h"
#include "sysdep.h"
#include "ptable.h"

extern Kernel *kernel;
extern Debug *debug;
extern FileSystem *fileSystem;
extern Semaphore *addrLock;
extern Bitmap *gPhysPageBitmap;
extern PTable *pTab;
#endif // MAIN_H

