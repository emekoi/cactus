/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include "wren.h"
#include "util.h"
#include <SDL2/SDL.h>

#define CACTUS_VERSION "0.3.0"
#define CLASS_NAME "Cactus"


static void w_cactus_getVersion(WrenVM *W) {
  wrenEnsureSlots(W, 1);
  wrenSetSlotString(W, 0, CACTUS_VERSION);
}


// void wren_open_system(WrenVM *W);
void wren_open_fs(WrenVM *W);
void wren_open_time(WrenVM *W);
void wren_open_graphics(WrenVM *W);
// void wren_open_audio(WrenVM *W);
void wren_open_mouse(WrenVM *W);
void wren_open_buffer(WrenVM *W);
// void wren_open_bufferfx(WrenVM *W);
void wren_open_font(WrenVM *W);
// void wren_open_source(WrenVM *W);
void wren_open_data(WrenVM *W);
// void wren_open_gif(WrenVM *W);

void wren_open_cactus(WrenVM *W) {
  WrenForeignMethodFn_Map *methods = wrenGetMethodMap(W);

  map_set(methods, "cactus" CLASS_NAME "versions", w_cactus_getVersion);

  /* Init submodules */
  static void (*classes[])(WrenVM*) = {
    /* Objects */
    wren_open_buffer,
    wren_open_font,
    // wren_open_source,
    wren_open_data,
    // wren_open_gif,
    /* Modules */
    // wren_open_system,
    wren_open_fs,
    wren_open_time,
    wren_open_graphics,
    // wren_open_audio,
    wren_open_mouse,
    // wren_open_bufferfx,
    NULL
  };

  for (int i = 0; classes[i]; i++) {
    classes[i](W);
  }
}
