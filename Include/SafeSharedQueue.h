#ifndef EXCERCISE_I_SAFESHAREDQUEUE_H
#define EXCERCISE_I_SAFESHAREDQUEUE_H

#include <semaphore.h>
#include <stdint.h>
#include "Attributes.h"
#include "Component.h"

typedef struct SafeSharedQueue SafeSharedQueue;

int createSafeSharedQueue(key_t key);
SafeSharedQueue *attachSharedQueue(int shmid);
SafeSharedQueue *getSafeSharedQueueByKey(key_t key);
int detachSharedQueue(SafeSharedQueue *sharedQueue);
int deleteSharedQueue(SafeSharedQueue* sharedQueue, int shmid);

void initializeSharedQueue(SafeSharedQueue *sharedQueue, size_t size) __NON_NULL(1);
uint8_t trySafeEnqueue(SafeSharedQueue *sharedQueue, Component *component) __NON_NULL(1, 2);
uint8_t trySafeDequeue(SafeSharedQueue *safeSharedQueue, Component *result) __NON_NULL(1, 2);

size_t getSafeSharedQueueSize(SafeSharedQueue *sharedQueue) __NON_NULL(1);
uint8_t isSafeSharedQueueEmpty(SafeSharedQueue *sharedQueue) __NON_NULL(1);
uint8_t isSafeSharedQueueFull(SafeSharedQueue *sharedQueue) __NON_NULL(1);

#endif //EXCERCISE_I_SAFESHAREDQUEUE_H
