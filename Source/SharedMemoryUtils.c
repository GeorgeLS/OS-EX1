#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/fcntl.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include "../Include/Utils.h"

int createSharedMemory(key_t key, size_t size) {
    if (key < 0) {
#if DEBUG
        report_error("createSharedMemory: Invalid key!");
#endif
        return -1;
    }
    return shmget(key, size, IPC_CREAT | 0666);
}

int getSharedMemory(key_t key, size_t size) {
    if (key < 0) {
#if DEBUG
        report_error("getSharedMemory: Invalid key!");
#endif
        return -1;
    }
    return shmget(key, size, 0666);
}

void *attachSharedMemory(int shmid) {
    return shmat(shmid, NULL, 0);
}

int detachSharedMemory(void *memory) {
    return shmdt(memory);
}

int deleteSharedMemory(int shmid) {
    return shmctl(shmid, IPC_RMID, 0);
}