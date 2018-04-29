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
#include "luax.h"
#include "m_buffer.h"

double m_graphics_maxFps = 60.;

static int inited = 0;
static int screenWidth = 0;
static int screenHeight = 0;
static int screenRef = 0;
static int fullscreen = 0;
static int resizable = 0;
static int borderless = 0;

SDL_Window *m_graphics_window;
Buffer *m_graphics_screen;


static void resetVideoMode(lua_State *L) {
  UNUSED(L);
  /* Reset video mode */
  SDL_SetWindowFullscreen(m_graphics_window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
  SDL_SetWindowSize(m_graphics_window, screenWidth, screenHeight);
  SDL_SetWindowResizable(m_graphics_window, resizable ? SDL_TRUE : SDL_FALSE);
  SDL_SetWindowBordered(m_graphics_window, borderless ? SDL_FALSE : SDL_TRUE);

  /* Reset screen buffer */
  if (m_graphics_screen) {
    sr_Buffer *b = m_graphics_screen->buffer;
    b->pixels = (void*) SDL_GetWindowSurface(m_graphics_window)->pixels;
    b->w = screenWidth;
    b->h = screenHeight;
    sr_setClip(b, sr_rect(0, 0, b->w, b->h));
  }
}


static int l_graphics_init(lua_State *L) {
  screenWidth = luaL_checkint(L, 1);
  screenHeight = luaL_checkint(L, 2);
  const char *title = luaL_optstring(L, 3, "sol");
  fullscreen = luax_optboolean(L, 4, 0);
  resizable = luax_optboolean(L, 5, 0);
  borderless = luax_optboolean(L, 6, 0);

  if (inited) {
    luaL_error(L, "graphics are already inited");
  }
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    luaL_error(L, "could not init video");
  }
  /* Create the main window */
  m_graphics_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
  if (!m_graphics_window) {
    luaL_error(L, "could not create window %s", SDL_GetError());
  }
  /* Init SDL video */
  resetVideoMode(L);
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


static int l_graphics_setSize(lua_State *L) {
  screenWidth = luaL_optnumber(L, 1, screenWidth);
  screenHeight = luaL_optnumber(L, 2, screenHeight);
  resetVideoMode(L);
  return 0;
}

static int l_graphics_setFullscreen(lua_State *L) {
  fullscreen = luax_optboolean(L, 1, 0);
  resetVideoMode(L);
  return 0;
}

static int l_graphics_getFullscreen(lua_State *L) {
  lua_pushboolean(L, fullscreen);
  return 1;
}

static int l_graphics_setMaxFps(lua_State *L) {
  m_graphics_maxFps = luaL_optnumber(L, 1, 60);
  return 0;
}

static int l_graphics_getMaxFps(lua_State *L) {
  lua_pushnumber(L, m_graphics_maxFps);
  return 1;
}

int luaopen_graphics(lua_State *L) {
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
