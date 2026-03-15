#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>

typedef unsigned int uint;

typedef enum {
    e_failure = 0,
    e_success = 1
} Status;

typedef enum {
    e_unsupported = 0,
    e_encode,
    e_decode
} OperationType;

#endif /* TYPES_H */
