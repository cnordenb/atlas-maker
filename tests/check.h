#ifndef CHECK_H
#define CHECK_H

#include <stdio.h>

/* Counts failed checks; a test program returns it from main so CTest sees the failure. */
static int check_failures;

#define CHECK(cond)                                                                 \
    do {                                                                            \
        if (!(cond)) {                                                              \
            fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, __LINE__, #cond); \
            check_failures++;                                                       \
        }                                                                           \
    } while (0)

#endif
