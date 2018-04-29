/**
 * Copyright (c) 2015 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include "lib/sera/sera.h"
#include "util.h"
#include "luax.h"
#include "m_source.h"
#include "m_buffer.h"
#include "fs.h"

extern double m_graphics_maxFps;
extern Buffer* m_graphics_screen;
extern SDL_Window *m_graphics_window;

static lua_State *L;
static SDL_mutex *luaMutex;

static void shutdown(void) {
#ifndef __APPLE__
  SDL_UnlockMutex(luaMutex);
  SDL_Quit();
#endif
  SDL_DestroyWindow(m_graphics_window);
}

int luaopen_sol(lua_State *L);

int main(int argc, char **argv) {
  atexit(shutdown);

  /* Init lua state mutex and pass to sources module */
  luaMutex = SDL_CreateMutex();
  ASSERT(luaMutex);
  source_setLuaMutex(luaMutex);

  /* Init lua state */
  L = luaL_newstate();
  luaL_openlibs(L);

  /* Init main module -- this also inits the submodules */
  luaL_requiref(L, "sol", luaopen_sol, 1);

  /* Push command line arguments */
  lua_getglobal(L, "sol");
  if (!lua_isnil(L, -1)) {
    lua_newtable(L);
    int i;
    for (i = 0; i < argc; i++) {
      lua_pushstring(L, argv[i]);
      lua_rawseti(L, -2, i + 1);
    }
    lua_setfield(L, -2, "_argv");
  }
  lua_pop(L, 1);

  /* Init embedded scripts -- these should be ordered in the array in the order
   * we want them loaded; init.lua should always be last since it depends on
   * all the other modules */
  #include "buffer_lua.h"
  #include "time_lua.h"
  #include "graphics_lua.h"
  #include "keyboard_lua.h"
  #include "mouse_lua.h"
  #include "debug_lua.h"
  #include "system_lua.h"
  #include "init_lua.h"

  struct {
    const char *name, *data; int size;
  } items[] = {
    { "buffer.lua",     buffer_lua,     sizeof(buffer_lua)    },
    { "time.lua",       time_lua,       sizeof(time_lua)      },
    { "graphics.lua",   graphics_lua,   sizeof(graphics_lua)  },
    { "keyboard.lua",   keyboard_lua,   sizeof(keyboard_lua)  },
    { "mouse.lua",      mouse_lua,      sizeof(mouse_lua)     },
    { "debug.lua",      debug_lua,      sizeof(debug_lua)     },
    { "system.lua",     system_lua,     sizeof(system_lua)    },
    { "init.lua",       init_lua,       sizeof(init_lua)      },
    { NULL, NULL, 0 }
  };

  ASSERT(SDL_LockMutex(luaMutex) == 0);
  int i;
  for (i = 0; items[i].name; i++) {
    int err = luaL_loadbuffer(L, items[i].data, items[i].size, items[i].name);
    if (err || lua_pcall(L, 0, 0, 0) != 0) {
      const char *str = lua_tostring(L, -1);
      fprintf(stderr, "error: %s\n", str);
      abort();
    }
  }
  ASSERT(SDL_UnlockMutex(luaMutex) == 0);

  /* Do main loop */
  double last = 0;
  SDL_Surface *screen;
  for (;;) {
    screen = SDL_GetWindowSurface(m_graphics_window);
    if (screen && SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
    ASSERT(SDL_LockMutex(luaMutex) == 0);
    lua_getglobal(L, "sol");
    if (!lua_isnil(L, -1)) {
      lua_getfield(L, -1, "_onStep");
      if (!lua_isnil(L, -1)) {
        int err = lua_pcall(L, 0, 0, 0);
        if (err) {
          const char *str = lua_tostring(L, -1);
          fprintf(stderr, "error: %s\n", str);
          abort();
        }
      }
      lua_pop(L, 1);
    }
    ASSERT(SDL_UnlockMutex(luaMutex) == 0);
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
