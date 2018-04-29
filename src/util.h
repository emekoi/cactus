/**
 * Copyright (c) 2015 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#define ASSERT(x)\
  do {\
    if (!(x)) {\
      fprintf(stderr, "%s:%d: %s(): assertion '%s' failed\n",\
              __FILE__, __LINE__, __func__, #x);\
      abort();\
    }\
  } while (0)

#define DEBUG() do {\
  fprintf(stdout, "[TRACE]: %s:%d %s(): %d\n", __FILE__, __LINE__, __func__); \
} while (0)

#define TRACE(...) do { \
  fprintf(stdout, "[TRACE]: %s:%d %s(): ", __FILE__, __LINE__, __func__); \
  fprintf(stdout, __VA_ARGS__); \
  fprintf(stdout, "\n"); \
} while(0);

#define UNUSED(x)       ((void) (x))
#define MIN(a, b)       ((b) < (a) ? (b) : (a))
#define MAX(a, b)       ((b) > (a) ? (b) : (a))
#define CLAMP(x, a, b)  (MAX(a, MIN(x, b)))
#define LERP(a, b, p)   ((a) + ((b) - (a)) * (p))

#endif
