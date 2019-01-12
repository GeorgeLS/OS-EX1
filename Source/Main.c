#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include "../Include/Macros.h"
#include "../Include/Attributes.h"
#include "../Include/Processes.h"
#include "../Include/SafeSharedQueue.h"
#include "../Include/Utils.h"

#define RESOURCES_N 7
#define PROCESSES_N 8

typedef void (*function)(void);

function processes[] = {
        firstComponentCreator,
        secondComponentCreator,
        thirdComponentCreator,
        paint,
        firstComponentChecker,
        secondComponentChecker,
        thirdComponentChecker,
        composer
};

const char *paths[] = {
        CREATE_PAINT_PATH,
        PAINT_CHECK_PATH1,
        PAINT_CHECK_PATH2,
        PAINT_CHECK_PATH3,
        CHECK_COMPOSE_PATH1,
        CHECK_COMPOSE_PATH2,
        CHECK_COMPOSE_PATH3
};

typedef struct {
    int shmid;
    SafeSharedQueue *sharedQueue;
} Resource;

Resource resources[RESOURCES_N];
size_t resourceIndex;

__INLINE static inline size_t getQueueSize(char *str) {
    return (size_t) strtol(str, NULL, 10);
}

__INLINE static inline void usage(void) {
    fprintf(stderr, "Please provide the number of components to be created!\n");
    fprintf(stderr, "Usage: ./program <Number of components>\n");
}

__INLINE static inline void addResouce(int shmid, SafeSharedQueue *sharedQueue) {
    resources[resourceIndex].shmid = shmid;
    resources[resourceIndex].sharedQueue = sharedQueue;
    ++resourceIndex;
}

static void allocateResources(size_t size) {
    size_t numberOfPaths = ARRAY_ELEMENTS(paths);
    for (size_t i = 0U; i != numberOfPaths; ++i) {
        int shmid = createSafeSharedQueue(getKeyByPath(paths[i]));
        SafeSharedQueue *sharedQueue = attachSharedQueue(shmid);
        initializeSharedQueue(sharedQueue, size);
        addResouce(shmid, sharedQueue);
    }
}

static void cleanupResources(void) {
    for (size_t i = 0U; i != resourceIndex; ++i) {
        deleteSharedQueue(resources[i].sharedQueue, resources[i].shmid);
        detachSharedQueue(resources[i].sharedQueue);
    }
}

static void createProcesses(void) {
    pid_t pids[PROCESSES_N];
    for (size_t i = 0U; i != PROCESSES_N; ++i) {
        pids[i] = fork();
        if (pids[i] < 0) exit(EXIT_FAILURE);
        else if (pids[i] == 0) processes[i]();
    }
}

static void waitProcesses(void) {
    for (size_t i = 0U; i != PROCESSES_N; ++i) {
        int status;
        wait(&status);
    }
}

static int run(int argc, char *argv[]) {
    if (argc < 2) { usage(); return EXIT_FAILURE; }
    allocateResources(getQueueSize(argv[1]));
    createProcesses();
    waitProcesses();
    cleanupResources();
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) { return run(argc, argv); }