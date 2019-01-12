#include <semaphore.h>
#include <stdint.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "../Include/Macros.h"
#include "../Include/Utils.h"
#include "../Include/SharedMemoryUtils.h"
#include "../Include/Component.h"

typedef struct {
    ssize_t front;
    ssize_t rear;
    sem_t mutex;
    int shmDataID;
    Component *components;
} SafeSharedQueueInfo;

typedef struct SafeSharedQueue {
    size_t size;
    int queueInfoMemoryID;
    SafeSharedQueueInfo *queueInfo;
} SafeSharedQueue;

__INLINE static inline int createSafeSharedQueueInfo(void) {
    return createSharedMemory(IPC_PRIVATE, sizeof(SafeSharedQueueInfo));
}

__INLINE static inline SafeSharedQueueInfo *attachSafeSharedQueueInfo(int shmid, size_t size) {
    SafeSharedQueueInfo *info = attachSharedMemory(shmid);
    sem_init(&info->mutex, 1, 1);
    info->front = info->rear = -1;
    info->shmDataID = createSharedMemory(IPC_PRIVATE, size * sizeof(Component));
    info->components = attachSharedMemory(info->shmDataID);
    return info;
}

__INLINE static inline int detachSafeSharedQueueInfo(SafeSharedQueueInfo *info) {
    detachSharedMemory(info->components);
    return detachSharedMemory(info);
}

__INLINE static inline int deleteSafeSharedQueueInfo(SafeSharedQueueInfo *info, int shmid) {
    sem_destroy(&info->mutex);
    deleteSharedMemory(info->shmDataID);
    return deleteSharedMemory(shmid);
}

int createSafeSharedQueue(key_t key) {
    return createSharedMemory(key, sizeof(SafeSharedQueue));
}

SafeSharedQueue *attachSharedQueue(int shmid) {
    return attachSharedMemory(shmid);
}

SafeSharedQueue *getSafeSharedQueueByKey(key_t key) {
    return attachSharedMemory(getSharedMemory(key, sizeof(SafeSharedQueue)));
}

int detachSharedQueue(SafeSharedQueue *sharedQueue) {
    detachSafeSharedQueueInfo(sharedQueue->queueInfo);
    return detachSharedMemory(sharedQueue);
}

int deleteSharedQueue(SafeSharedQueue* sharedQueue, int shmid) {
    deleteSafeSharedQueueInfo(sharedQueue->queueInfo, sharedQueue->queueInfoMemoryID);
    return deleteSharedMemory(shmid);
}

void initializeSharedQueue(SafeSharedQueue *sharedQueue, size_t size) {
    sharedQueue->size = size;
    sharedQueue->queueInfoMemoryID = createSafeSharedQueueInfo();
    sharedQueue->queueInfo = attachSafeSharedQueueInfo(sharedQueue->queueInfoMemoryID, size);
}

uint8_t isSafeSharedQueueEmpty(SafeSharedQueue *sharedQueue) {
    return sharedQueue->queueInfo->front == -1 ? 1U : 0U;
}

uint8_t isSafeSharedQueueFull(SafeSharedQueue *sharedQueue) {
    register ssize_t front = sharedQueue->queueInfo->front;
    register ssize_t rear = sharedQueue->queueInfo->rear;
    return ((front == 0 && rear == sharedQueue->size - 1) || front == rear + 1) ? 1U : 0U;
}

uint8_t trySafeEnqueue(SafeSharedQueue *sharedQueue, Component *component) {
    uint8_t status = 0U;
    SafeSharedQueueInfo *info = sharedQueue->queueInfo;
    sem_wait(&info->mutex);
    if (isSafeSharedQueueFull(sharedQueue)) {
#if DEBUG
        report_error("safeEnqueue: Queue is full");
#endif
        goto __EXIT;
    }
    if (info->front == -1) info->front = 0;
    info->rear = (info->rear + 1) % sharedQueue->size;
    info->components[info->rear] = *component;
    status = 1U;
__EXIT:
    sem_post(&info->mutex);
    return status;
}

uint8_t trySafeDequeue(SafeSharedQueue *sharedQueue, Component *result) {
    uint8_t status = 0U;
    SafeSharedQueueInfo *info = sharedQueue->queueInfo;
    sem_wait(&info->mutex);
    *result = (Component) {0, 0};
    if (isSafeSharedQueueEmpty(sharedQueue)) {
#if DEBUG
        report_error("safeDequeue: Queue is empty");
#endif
        goto __EXIT;
    }
    *result = info->components[info->front];
    if (info->front == info->rear) info->front = info->rear = -1;
    else info->front = (info->front + 1) % sharedQueue->size;
    status = 1U;
    __EXIT:
    sem_post(&info->mutex);
    return status;
}

size_t getSafeSharedQueueSize(SafeSharedQueue *sharedQueue) {
    return sharedQueue->size;
}