#include <sys/types.h>

#ifndef EXCERCISE_I_SHAREDMEMORYUTILS_H
#define EXCERCISE_I_SHAREDMEMORYUTILS_H

int createSharedMemory(key_t key, size_t size);
int getSharedMemory(key_t key, size_t size);
void *attachSharedMemory(int shmid);
int detachSharedMemory(void *memory);
int deleteSharedMemory(int shmid);

#endif //EXCERCISE_I_SHAREDMEMORYUTILS_H
