/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#pragma once

#include <stdio.h>
#include "wren.h"
#include "util.h"
#include "fs.h"

static char *concat(const char *str, ...) {
  va_list args;
  const char *s;
  /* Get len */
  int len = strlen(str);
  va_start(args, str);
  while ((s = va_arg(args, char*))) {
    len += strlen(s);
  }
  va_end(args);
  /* Build string */
  char *res = malloc(len + 1);
  if (!res) return NULL;
  strcpy(res, str);
  va_start(args, str);
  while ((s = va_arg(args, char*))) {
    strcat(res, s);
  }
  va_end(args);
  return res;
}

static void wren_writeFn(WrenVM *vm, const char *text) {
  UNUSED(vm);
	printf("%s", text);
}


static void wren_errorFn(
  WrenVM* vm, WrenErrorType type, const char* module,
  int line, const char* message
) {
  UNUSED(vm);
  switch (type) {
    case WREN_ERROR_COMPILE: {
      fprintf(
        stderr, "compile error:\n  %s:%d: %s\n",
        module, line, message);
      break;
    }

    case WREN_ERROR_RUNTIME: {
      fprintf(stderr, "runtime error:\n  %s:%d: %s\ntraceback:\n", module, line, message);
      break;
    }

    case WREN_ERROR_STACK_TRACE: {
      fprintf(stderr, "   [%s] %s:%d\n", module, message, line);
      break;
    }
  }
}


static char *wren_loadModuleFn(WrenVM *vm, const char *name) {
  UNUSED(vm);
  return fs_read(name, NULL);
}


static WrenForeignMethodFn wren_bindForeignMethod(
	WrenVM* vm, const char* module, const char* className,
  bool isStatic, const char* signature
) {
  UNUSED(module);
  // char *fullSignature = concat(module, className, signature, isStatic ? "s" : "", NULL);
	char *fullSignature = concat("cactus", className, signature, isStatic ? "s" : "", NULL);
	WrenForeignMethodFn_Map *map = wrenGetMethodMap(vm);
	WrenForeignMethodFn *method = map_get(map, fullSignature);
	free(fullSignature);
	return method ? *method : NULL;
}

static WrenForeignClassMethods wren_bindForeignClass(
  WrenVM* vm, const char* module, const char* className
) {
  UNUSED(module);
  // char *fullSignature = concat(module, className, NULL);
  char *fullSignature = concat("cactus", className, NULL);
  WrenForeignClassMethods_Map *map = wrenGetClassMap(vm);
  WrenForeignClassMethods *methods = map_get(map, fullSignature);
  free(fullSignature);
  return methods ? *methods : (WrenForeignClassMethods){ NULL, NULL};
}
