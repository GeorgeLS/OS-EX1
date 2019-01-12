#ifndef EXCERCISE_I_MACROS_H
#define EXCERCISE_I_MACROS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <time.h>

#define ARRAY_ELEMENTS(array) (sizeof(array) / sizeof(array[0]))
#define MIN(x, y, z) (((x) < (y)) ? (((x) < (z)) ? (x) : (z)) : (((y) < (z)) ? (y) : (z)))
#define __MALLOC(size, type) ((type*) malloc((size) * sizeof(type)))
#define __SLEEP(ms) { nanosleep((const struct timespec[]) {{0U, 1000000UL * (ms)}}, NULL); }

#define __TIME_MS(ms) { \
    struct timespec tspec; \
    clock_gettime(CLOCK_MONOTONIC, &tspec); \
    ms = tspec.tv_sec * 1000UL + tspec.tv_nsec / 1000000UL; \
} \

#define RANDOM(min, max, result) { \
    const unsigned long range = max - min + 1; \
    const unsigned long buckets = (unsigned long) RAND_MAX / range; \
    const  unsigned long limit = buckets * range; \
    unsigned long n; \
    do { n = random(); } while (n > limit); \
    result = n / buckets; \
} \

#define RANDOM_SLEEP_TIME(max, result) RANDOM(1, max, result)
#define GENERATE_ID(id) { snprintf(id, 5, "%ld", (1000 + (random() % 9000))); }
#define GENERATE_KEY(path, id) (ftok(path, id));

#endif //EXCERCISE_I_MACROS_H
