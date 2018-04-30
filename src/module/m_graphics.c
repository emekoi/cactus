/**
 * Copyright (c) 2015 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <SDL2/SDL.h>
#include "lib/sera/sera.h"
#include "util.h"
#include "wren.h"
// #include "m_buffer.h"

double m_graphics_maxFps = 60.;

static int inited = 0;
static int screenWidth = 0;
static int screenHeight = 0;
static int screenRef = 0;
static int fullscreen = 0;
static int resizable = 0;
static int borderless = 0;

SDL_Window *m_graphics_window;
// Buffer *m_graphics_screen;


static void resetVideoMode(WrenVM *W) {
  UNUSED(W);
  /* Reset video mode */
  SDL_SetWindowFullscreen(m_graphics_window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
  SDL_SetWindowSize(m_graphics_window, screenWidth, screenHeight);
  SDL_SetWindowResizable(m_graphics_window, resizable ? SDL_TRUE : SDL_FALSE);
  SDL_SetWindowBordered(m_graphics_window, borderless ? SDL_FALSE : SDL_TRUE);

  /* Reset screen buffer */
  // if (m_graphics_screen) {
  //   sr_Buffer *b = m_graphics_screen->buffer;
  //   b->pixels = (void*) SDL_GetWindowSurface(m_graphics_window)->pixels;
  //   b->w = screenWidth;
  //   b->h = screenHeight;
  //   sr_setClip(b, sr_rect(0, 0, b->w, b->h));
  // }
}

CHECK_TYPE(W, WREN_TYPE_NUM, 1, "expected Num");

static void w_graphics_init(WrenVM *W) {
  CHECK_TYPE(W, WREN_TYPE_NUM, 1, "expected Num");
  screenWidth = wrenGetSlotDouble(W, 1);

  CHECK_TYPE(W, WREN_TYPE_NUM, 2, "expected Num");
  screenHeight = wrenGetSlotDouble(W, 2);

  CHECK_TYPE(W, WREN_TYPE_STRING, 3, "expected String");
  const char *title = wrenGetSlotString(W, 3);

  CHECK_TYPE(W, WREN_TYPE_BOOL, 4, "expected Bool");
  fullscreen = wrenGetSlotBool(W, 4, 0);

  CHECK_TYPE(W, WREN_TYPE_BOOL, 5, "expected Bool");
  resizable = wrenGetSlotBool(W, 5, 0);

  CHECK_TYPE(W, WREN_TYPE_BOOL, 6, "expected Bool");
  borderless = wrenGetSlotBool(W, 6, 0);


  if (inited) {
    WREN_ERROR(W, "graphics are already inited");
  }

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    WREN_ERROR(W, "could not init video");
  }

  /* Create the main window */
  m_graphics_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
  if (!m_graphics_window) {
    WREN_ERROR(W, "could not create window %s", SDL_GetError());
  }
  /* Init SDL video */
  resetVideoMode(W);
  /* Create, store in registry and return main screen buffer */
  m_graphics_screen = buffer_new(L);
  m_graphics_screen->buffer = sr_newBufferShared(
    SDL_GetWindowSurface(m_graphics_window)->pixels, screenWidth, screenHeight);
  lua_pushvalue(L, -1);
  screenRef = lua_ref(L, LUA_REGISTRYINDEX);
  /* Set state */
  inited = 1;
  return 1;
}


static int l_graphics_setSize(WrenVM *W) {
  screenWidth = luaL_optnumber(L, 1, screenWidth);
  screenHeight = luaL_optnumber(L, 2, screenHeight);
  resetVideoMode(L);
  return 0;
}

static int l_graphics_setFullscreen(WrenVM *W) {
  fullscreen = luax_optboolean(L, 1, 0);
  resetVideoMode(L);
  return 0;
}

static int l_graphics_getFullscreen(WrenVM *W) {
  lua_pushboolean(L, fullscreen);
  return 1;
}

static int l_graphics_setMaxFps(WrenVM *W) {
  m_graphics_maxFps = luaL_optnumber(L, 1, 60);
  return 0;
}

static int l_graphics_getMaxFps(WrenVM *W) {
  lua_pushnumber(L, m_graphics_maxFps);
  return 1;
}

int luaopen_graphics(WrenVM *W) {
  luaL_Reg reg[] = {
    { "init",           l_graphics_init           },
    { "setSize",        l_graphics_setSize        },
    { "setFullscreen",  l_graphics_setFullscreen  },
    { "getFullscreen",  l_graphics_getFullscreen  },
    { "setMaxFps",      l_graphics_setMaxFps      },
    { "getMaxFps",      l_graphics_getMaxFps      },
    { NULL, NULL }
  };
  luaL_newlib(L, reg);
  return 1;
}
