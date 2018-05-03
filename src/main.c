/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "lib/sera/sera.h"
#include "lib/map/map.h"
#include "util.h"
#include "conf.h"
#include "wren.h"
// #include "m_source.h"
// #include "m_buffer.h"
#include "fs.h"

extern double m_graphics_maxFps;
// extern Buffer* m_graphics_screen;
extern SDL_Window *m_graphics_window;

static WrenVM *W;
static ForeignWrenData foreignData;
static WrenConfiguration configuration;
// static SDL_mutex *luaMutex;

static void shutdown(void) {
#ifndef __APPLE__
  // SDL_UnlockMutex(luaMutex);
  SDL_Quit();
#endif
  wrenFreeVM(W);
  map_deinit(&(foreignData.methods));
  map_deinit(&(foreignData.classes));
  SDL_DestroyWindow(m_graphics_window);
}

void wren_open_cactus(WrenVM *W);

int main(int argc, char **argv) {
  UNUSED(argc);
  UNUSED(argv);
  atexit(shutdown);

  /* Init lua state mutex and pass to sources module */
  // luaMutex = SDL_CreateMutex();
  // ASSERT(luaMutex);
  // source_setLuaMutex(luaMutex);
  /* Init our foreign data for Wren */

  map_init(&(foreignData.methods));
  map_init(&(foreignData.classes));

  /* Configure Wren VM */
  wrenInitConfiguration(&configuration);

  configuration.userData = &foreignData;
  configuration.writeFn = wren_writeFn;
  configuration.errorFn = wren_errorFn;
  configuration.loadModuleFn = wren_loadModuleFn;
  configuration.bindForeignMethodFn = wren_bindForeignMethod;
  configuration.bindForeignClassFn = wren_bindForeignClass;

  /* Init Wren VM */
  W = wrenNewVM(&configuration);

  /* Init main module -- this also inits the submodules */
  wren_open_cactus(W);

  /* Push command line arguments */
  // lua_getglobal(L, "sol");
  // if (!lua_isnil(L, -1)) {
  //   lua_newtable(L);
  //   int i;
  //   for (i = 0; i < argc; i++) {
  //     lua_pushstring(L, argv[i]);
  //     lua_rawseti(L, -2, i + 1);
  //   }
  //   lua_setfield(L, -2, "_argv");
  // }
  // lua_pop(L, 1);

  /* Init embedded scripts -- these should be ordered in the array in the order
   * we want them loaded; init.lua should always be last since it depends on
   * all the other modules */
  #include "cactus_wren.h"

  struct {
    const char *name, *data; int size;
  } items[] = {
    { "cactus.wren", cactus_wren, sizeof(cactus_wren) },
    { NULL, NULL, 0 }
  };

  // ASSERT(SDL_LockMutex(luaMutex) == 0);
  for (int i = 0; items[i].name; i++) {
    int err = wrenInterpret(W, items[i].data);
    if (err != WREN_RESULT_SUCCESS) abort();
  }
  // ASSERT(SDL_UnlockMutex(luaMutex) == 0);

  /* Do main loop */
  double last = 0;
  SDL_Surface *screen;
  for (;;) {
    screen = SDL_GetWindowSurface(m_graphics_window);
    if (screen && SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
    // ASSERT(SDL_LockMutex(luaMutex) == 0);
    // lua_getglobal(L, "sol");
    // if (!lua_isnil(L, -1)) {
    //   lua_getfield(L, -1, "_onStep");
    //   if (!lua_isnil(L, -1)) {
    //     int err = lua_pcall(L, 0, 0, 0);
    //     if (err) {
    //       const char *str = lua_tostring(L, -1);
    //       fprintf(stderr, "error: %s\n", str);
    //       abort();
    //     }
    //   }
    //   lua_pop(L, 1);
    // }
    // ASSERT(SDL_UnlockMutex(luaMutex) == 0);
    if (screen && SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
    SDL_Surface *screen = SDL_GetWindowSurface(m_graphics_window);
    if (screen) {
      SDL_UpdateWindowSurface(m_graphics_window);
    }
    /* Wait for next frame */
    double step = (1. / m_graphics_maxFps);
    double now = SDL_GetTicks() / 1000.;
    double wait = step - (now - last);
    last += step;
    if (wait > 0) {
      SDL_Delay(wait * 1000.);
    } else {
      last = now;
    }
  }

  return 0;
}
