/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include "wren.h"
#include <SDL2/SDL.h>

#define CACTUS_VERSION "0.3.0"
#define CACTUS_CLASS_NAME "Cactus"

static void w_cactus_getVersion(WrenVM *W) {
  wrenEnsureSlots(W, 1);
  wrenSetSlotString(W, 0, CACTUS_VERSION);
}


// int wren_open_system(WrenVM *W);
// int wren_open_fs(WrenVM *W);
int wren_open_time(WrenVM *W);
// int wren_open_graphics(WrenVM *W);
// int wren_open_audio(WrenVM *W);
int wren_open_mouse(WrenVM *W);
// int wren_open_buffer(WrenVM *W);
// int wren_open_bufferfx(WrenVM *W);
// int wren_open_font(WrenVM *W);
// int wren_open_source(WrenVM *W);
// int wren_open_data(WrenVM *W);
// int wren_open_gif(WrenVM *W);

void wren_open_cactus(WrenForeignMethodFn_Map *map) {
  map_set(map, "cactus" CACTUS_CLASS_NAME "versions", w_cactus_getVersion);

  /* Init submodules */
 void (*fn)(WrenForeignMethodFn_Map *map) classes[] = {
    /* Objects */
    // wren_open_font,
    // wren_open_buffer,
    // wren_open_source,
    // wren_open_data,
    // wren_open_gif,
    /* Modules */
    // wren_open_system,
    // wren_open_fs,
    wren_open_time,
    // wren_open_graphics,
    // wren_open_audio,
    wren_open_mouse,
    // wren_open_bufferfx,
    NULL
  };

  for (i = 0; mods[i]; i++) {
    mods[i](map);
  }
}
