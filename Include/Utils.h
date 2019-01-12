#ifndef EXCERCISE_I_UTILS_H
#define EXCERCISE_I_UTILS_H

#include <semaphore.h>
#include "Attributes.h"

void report_error(char *fmt, ...) __FORMAT(vfprintf, 1, 2);
key_t getKeyByPath(const char *path);

#endif //EXCERCISE_I_UTILS_H
