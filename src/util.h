/**
 * Copyright (c) 2015 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <string.h>
#include "wren.h"
#include "lib/map/map.h"

typedef map_t(WrenForeignMethodFn) WrenForeignMethodFn_Map;
typedef map_t(WrenForeignClassMethods) WrenForeignClassMethods_Map;

typedef struct {
  WrenForeignMethodFn_Map methods;
  WrenForeignClassMethods_Map classes;
} ForeignWrenData;




{
  char *stringf(const char *str, ...) {

  return v;
}
}




#define WREN_ERROR(vm, str, ...) do {      \
  va_list arg, tmp;                        \
  char *res;                               \
  int len;                                 \
  if (str == NULL) return NULL;            \
  va_start(arg, str);                      \
  /* create a copy of the list of args */  \
  __va_copy(tmp, arg);                     \
  /* get length string should be */        \
  len = vsnprintf(res, 0, str, tmp);       \
  /* toss temp copy */                     \
  va_end(tmp);                             \
  /* something is wrong... */              \
  if (len < 0) return NULL;                \
  /* resize the string */                  \
  res = calloc(len + 1, sizeof(char));     \
  res[len] = '\0';                         \
  /* format the string */                  \
  vsnprintf(res, len + 1, str, arg);       \
  /* toss args */                          \
  va_end(arg);                             \
  wrenSetSlotString(res);                  \
  wrenAbortFiber(vm, 0);                   \
} while (false)

#define CHECK_TYPE(vm, type, slot, msg) do { \
  if (wrenGetSlotType(vm, slot) != type) {   \
    wrenSetSlotString(msg);                  \
    wrenAbortFiber(vm, 0);                   \
  }                                          \
} while (false)

#define ASSERT(x)\
  do {\
    if (!(x)) {\
      fprintf(stderr, "%s:%d: %s(): assertion '%s' failed\n",\
              __FILE__, __LINE__, __func__, #x);\
      abort();\
    }\
  } while (false)

#define DEBUG() do {\
  fprintf(stdout, "[TRACE]: %s:%d %s(): %d\n", __FILE__, __LINE__, __func__); \
} while (false)

#define TRACE(...) do { \
  fprintf(stdout, "[TRACE]: %s:%d %s(): ", __FILE__, __LINE__, __func__); \
  fprintf(stdout, __VA_ARGS__); \
  fprintf(stdout, "\n"); \
} while(false);

#define UNUSED(x)       ((void) (x))
#define MIN(a, b)       ((b) < (a) ? (b) : (a))
#define MAX(a, b)       ((b) > (a) ? (b) : (a))
#define CLAMP(x, a, b)  (MAX(a, MIN(x, b)))
#define LERP(a, b, p)   ((a) + ((b) - (a)) * (p))

#endif