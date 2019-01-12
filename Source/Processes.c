#include <inttypes.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "../Include/Macros.h"
#include "../Include/SafeSharedQueue.h"
#include "../Include/Component.h"
#include "../Include/Utils.h"
#include "../Include/SharedMemoryUtils.h"
#include "../Include/Processes.h"

// These times are in ms
#define MAX_SLEEP_TIME 10U
#define TYPE1_PAINT_TIME 2U
#define TYPE1_CHECK_TIME 3U
#define TYPE2_PAINT_TIME 5U
#define TYPE2_CHECK_TIME 3U
#define TYPE3_PAINT_TIME 10U
#define TYPE3_CHECK_TIME 7U
#define PRODUCT_COMPOSE_TIME 12U

#define CREATE_PRODUCT_ID(res, id1, id2, id3) (snprintf(res, 13, "%s%s%s", id1, id2, id3))

__INLINE static inline uint32_t getComponentPaintTime(ComponentType type) {
    switch (type) {
        case Type1: return TYPE1_PAINT_TIME;
        case Type2: return TYPE2_PAINT_TIME;
        case Type3: return TYPE3_PAINT_TIME;
    }
}

__INLINE static inline uint32_t getComponentCheckTime(ComponentType type) {
    switch (type) {
        case Type1: return TYPE1_CHECK_TIME;
        case Type2: return TYPE2_CHECK_TIME;
        case Type3: return TYPE3_CHECK_TIME;
    }
}

__NO_RETURN static void createComponentProcess(ComponentType type) {
    srandom((unsigned int) (time(NULL) ^ getpid()));
    SafeSharedQueue *sharedQueue = getSafeSharedQueueByKey(getKeyByPath(CREATE_PAINT_PATH));
    size_t componentsToCreate = getSafeSharedQueueSize(sharedQueue);
    while (componentsToCreate--) {
        unsigned long randomSleepTime;
        RANDOM_SLEEP_TIME(MAX_SLEEP_TIME, randomSleepTime);
        __SLEEP(randomSleepTime);
        Component *component = createComponent(type);
        while (!trySafeEnqueue(sharedQueue, component)) {};
    }
    detachSharedQueue(sharedQueue);
    exit(EXIT_SUCCESS);
}

__NO_RETURN void firstComponentCreator(void) { createComponentProcess(Type1); }

__NO_RETURN void secondComponentCreator(void) { createComponentProcess(Type2); }

__NO_RETURN void thirdComponentCreator(void) { createComponentProcess(Type3); }

__NO_RETURN void paint(void) {
    SafeSharedQueue *createPaintSharedQueue = getSafeSharedQueueByKey(getKeyByPath(CREATE_PAINT_PATH));
    SafeSharedQueue *checkSharedQueues[3] = {
            getSafeSharedQueueByKey(getKeyByPath(PAINT_CHECK_PATH1)),
            getSafeSharedQueueByKey(getKeyByPath(PAINT_CHECK_PATH2)),
            getSafeSharedQueueByKey(getKeyByPath(PAINT_CHECK_PATH3))
    };
    unsigned long totalPaintTime = 0UL;
    unsigned long previousTime;
    __TIME_MS(previousTime);
    size_t totalComponents;
    size_t componentsToPaint = totalComponents = getSafeSharedQueueSize(createPaintSharedQueue) * 3;
    while (componentsToPaint--) {
        Component component;
        while (!trySafeDequeue(createPaintSharedQueue, &component)) {};
        unsigned long now;
        __TIME_MS(now);
        totalPaintTime += now - previousTime;
        previousTime = now;
        __SLEEP(getComponentPaintTime(component.type));
        while (!trySafeEnqueue(checkSharedQueues[component.type], &component)) {}
    }
    for (size_t i = 0U; i != 3U; ++i) {
        detachSharedQueue(checkSharedQueues[i]);
    }
    detachSharedQueue(createPaintSharedQueue);
    printf("Average waiting time for painting = %.4fms\n", totalPaintTime / ((double) totalComponents));
    exit(EXIT_SUCCESS);
}

__INLINE static inline SafeSharedQueue *getCheckSafeSharedQueueByType(ComponentType type) {
    char *path;
    switch (type) {
        case Type1: path = PAINT_CHECK_PATH1; break;
        case Type2: path = PAINT_CHECK_PATH2; break;
        case Type3: path = PAINT_CHECK_PATH3; break;
    }
    return getSafeSharedQueueByKey(getKeyByPath(path));
}

__INLINE static inline SafeSharedQueue *getComposerSafeSharedQueueByType(ComponentType type) {
    char *path;
    switch (type) {
        case Type1: path = CHECK_COMPOSE_PATH1; break;
        case Type2: path = CHECK_COMPOSE_PATH2; break;
        case Type3: path = CHECK_COMPOSE_PATH3; break;
    }
    return getSafeSharedQueueByKey(getKeyByPath(path));
}

__NO_RETURN static void checkComponentProcess(ComponentType type) {
    SafeSharedQueue *paintCheckSharedQueue = getCheckSafeSharedQueueByType(type);
    SafeSharedQueue *checkComposeSharedQueue = getComposerSafeSharedQueueByType(type);
    size_t componentsToCheck = getSafeSharedQueueSize(paintCheckSharedQueue);
    while (componentsToCheck--) {
        Component component;
        while (!trySafeDequeue(paintCheckSharedQueue, &component)) {}
        __SLEEP(getComponentCheckTime(component.type));
        while (!trySafeEnqueue(checkComposeSharedQueue, &component)) {}
    }
    detachSharedQueue(paintCheckSharedQueue);
    detachSharedQueue(checkComposeSharedQueue);
    exit(EXIT_SUCCESS);
}

__NO_RETURN void firstComponentChecker(void) { checkComponentProcess(Type1); }

__NO_RETURN void secondComponentChecker(void) { checkComponentProcess(Type2); }

__NO_RETURN void thirdComponentChecker(void) { checkComponentProcess(Type3); }

__NO_RETURN void composer(void) {
    SafeSharedQueue *checkSharedQueues[3] = {
            getSafeSharedQueueByKey(getKeyByPath(CHECK_COMPOSE_PATH1)),
            getSafeSharedQueueByKey(getKeyByPath(CHECK_COMPOSE_PATH2)),
            getSafeSharedQueueByKey(getKeyByPath(CHECK_COMPOSE_PATH3))
    };
    unsigned long totalProductionTime = 0UL;
    size_t totalProducts = getSafeSharedQueueSize(checkSharedQueues[0]);
    size_t componentsToCompose = totalProducts * 3U;
    uint8_t componentTaken[3] = {0U};
    Component components[3];
    while (componentsToCompose--) {
        size_t index = 0U;
        while (componentTaken[index] || isSafeSharedQueueEmpty(checkSharedQueues[index])) {
            index = (index + 1U) % 3U;
        }
        Component component;
        while (!trySafeDequeue(checkSharedQueues[index], &component)) {}
        componentTaken[component.type] = 1U;
        components[component.type] = component;
        if (componentTaken[0] & componentTaken[1] & componentTaken[2]) {
            componentTaken[0] = componentTaken[1] = componentTaken[2] = 0U;
            __SLEEP(PRODUCT_COMPOSE_TIME);
            unsigned long timeTillProduction;
            __TIME_MS(timeTillProduction);
            timeTillProduction -= MIN(components[0].processingTime, components[1].processingTime, components[2].processingTime);
            totalProductionTime += timeTillProduction;
            char res[13];
            CREATE_PRODUCT_ID(res, components[0].id, components[1].id, components[2].id);
        }
    }
    for (size_t i = 0U; i != 3U; ++i) {
        detachSharedQueue(checkSharedQueues[i]);
    }
    printf("Average production time = %.4fms\n", totalProductionTime / ((double) totalProducts));
    exit(EXIT_SUCCESS);
}