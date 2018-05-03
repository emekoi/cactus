/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#ifdef _WIN32
  #include <windows.h>
#else
  #include <sys/time.h>
#endif
#include "util.h"
#include "wren.h"
#include <SDL2/SDL.h>
#include <time.h>

#define CLASS_NAME "Time"


static void w_time_getNow(WrenVM *W) {
  double t;
#ifdef _WIN32
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft);
  t = (ft.dwHighDateTime * 4294967296.0 / 1e7) + ft.dwLowDateTime / 1e7;
  t -= 11644473600.0;
#else
  struct timeval tv;
  gettimeofday(&tv, NULL);
  t = tv.tv_sec + tv.tv_usec / 1e6;
#endif
  wrenEnsureSlots(W, 1);
  wrenSetSlotDouble(W, 0, t);
}


static void w_time_getTime(WrenVM *W) {
  wrenEnsureSlots(W, 1);
  wrenSetSlotDouble(W, 0, SDL_GetTicks() / 1000.);
}


static void w_time_sleep(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_NUM, "expected Num");
  SDL_Delay(wrenGetSlotDouble(W, 1) * 1000.);
  wrenSetSlotBool(W, 0, true);
}


void wren_open_time(WrenVM *W) {
  WrenForeignMethodFn_Map *methods = wrenGetMethodMap(W);
  map_set(methods, "cactus" CLASS_NAME "nows",      w_time_getNow);
  map_set(methods, "cactus" CLASS_NAME "times",     w_time_getTime);
  map_set(methods, "cactus" CLASS_NAME "sleep(_)s", w_time_sleep);

  srand(time(NULL));
}
