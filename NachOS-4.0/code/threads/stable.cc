
#include "stable.h"

// Constructor
STable::STable() {
  this->bm = new Bitmap(MAX_SEMAPHORE);

  for (int i = 0; i < MAX_SEMAPHORE; i++) {
    this->semTab[i] = NULL;
  }
}

// Destructor
STable::~STable() {
  if (this->bm) {
    delete this->bm;
    this->bm = NULL;
  }
  for (int i = 0; i < MAX_SEMAPHORE; i++) {
    if (this->semTab[i]) {
      delete this->semTab[i];
      this->semTab[i] = NULL;
    }
  }
}

int STable::Create(char *name, int init) {

  // kiem tra da ton tai semaphore "name" chua
  for (int i = 0; i < MAX_SEMAPHORE; i++) {
    if (bm->Test(i)) {
      if (strcmp(name, semTab[i]->getName()) == 0) {
        return -1;
      }
    }
  }
  // Tim slot trong
  int id = this->FindFreeSlot();

  // Neu k tim thay thi tra ve -1
  if (id < 0) {
    return -1;
  }

  // Neu tim thay slot trong thi nap Semaphore vao semTab[id]
  this->semTab[id] = new Sem(name, init);
  return 0;
}

int STable::Wait(char *name) {
  for (int i = 0; i < MAX_SEMAPHORE; i++) {
    if (bm->Test(i)) {
      // So sanh name voi name cua semaphore trong semTab
      if (strcmp(name, semTab[i]->getName()) == 0) {
        // Neu ton tai thi cho semaphore down();
        semTab[i]->wait();
        return 0;
      }
    }
  }
  printf("Khong ton tai semaphore");
  return -1;
}

int STable::Signal(char *name) {
  for (int i = 0; i < MAX_SEMAPHORE; i++) {
    if (bm->Test(i)) {
      // so sanh name voi name cua semaphore trong semTab
      if (strcmp(name, semTab[i]->getName()) == 0) {
        // Neu ton tai thi cho semaphore up();
        semTab[i]->signal();
        return 0;
      }
    }
  }
  printf("Khong ton tai semaphore");
  return -1;
}

int STable::FindFreeSlot() { return this->bm->FindAndSet(); }
