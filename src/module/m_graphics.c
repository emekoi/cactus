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
#include "m_buffer.h"

#define CLASS_NAME "Graphics"


double m_graphics_maxFps = 60.;

static int inited = 0;
static int screenWidth = 0;
static int screenHeight = 0;
static int fullscreen = 0;
static int resizable = 0;
static int borderless = 0;

SDL_Window *m_graphics_window;
static Buffer *m_graphics_screen;
WrenHandle *screenHandle;


static void resetVideoMode(WrenVM *W) {
  UNUSED(W);
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


static void w_graphics_init(WrenVM *W) {
  wrenEnsureSlots(W, 7);
  wrenCheckSlot(W, 1, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 2, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 3, WREN_TYPE_STRING, "expected String");
  wrenCheckSlot(W, 4, WREN_TYPE_BOOL, "expected Bool");
  wrenCheckSlot(W, 5, WREN_TYPE_BOOL, "expected Bool");
  wrenCheckSlot(W, 6, WREN_TYPE_BOOL, "expected Bool");

  screenWidth = wrenGetSlotDouble(W, 1);
  screenHeight = wrenGetSlotDouble(W, 2);
  const char *title = wrenGetSlotString(W, 3);
  fullscreen = wrenGetSlotBool(W, 4);
  resizable = wrenGetSlotBool(W, 5);
  borderless = wrenGetSlotBool(W, 6);

  if (inited) {
    wrenError(W, "graphics are already inited");
  }

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    wrenError(W, "could not init video");
  }

  /* Create the main window */
  m_graphics_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);

  if (!m_graphics_window) {
    wrenError(W, "could not create window %s", SDL_GetError());
  }

  /* Init SDL video */
  resetVideoMode(W);
  /* Create, create handle and return main screen buffer */
  wrenGetVariable(W, "main", "Buffer", 0);
  buffer_new(W);
  m_graphics_screen = wrenGetSlotForeign(W, 0);
  m_graphics_screen->buffer = sr_newBufferShared(
    SDL_GetWindowSurface(m_graphics_window)->pixels, screenWidth, screenHeight);
  screenHandle = wrenGetSlotHandle(W, 0);

  /* Set state */
  inited = 1;
}


static void w_graphics_setSize(WrenVM *W) {
  wrenEnsureSlots(W, 3);
	wrenCheckSlot(W, 1, WREN_TYPE_NUM, "expected Num");
	wrenCheckSlot(W, 2, WREN_TYPE_NUM, "expected Num");

  screenWidth = wrenGetSlotDouble(W, 1);
  screenHeight = wrenGetSlotDouble(W, 2);
  resetVideoMode(W);
}


static void w_graphics_setFullscreen(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_BOOL, "expected Bool");
  fullscreen = wrenGetSlotBool(W, 1);
  resetVideoMode(W);
}


static void w_graphics_getFullscreen(WrenVM *W) {
  wrenEnsureSlots(W, 1);
  wrenSetSlotBool(W, 0, fullscreen);
}


static void w_graphics_setMaxFps(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, WREN_TYPE_NUM, 1, "expected Num");
  m_graphics_maxFps = wrenGetSlotDouble(W, 1);
}


static void w_graphics_getMaxFps(WrenVM *W) {
  wrenEnsureSlots(W, 1);
  wrenSetSlotDouble(W, 0, m_graphics_maxFps);
}


void wren_open_graphics(WrenVM *W) {
  WrenForeignMethodFn_Map *methods = wrenGetMethodMap(W);
  map_set(methods, "cactus" CLASS_NAME "init_(_,_,_,_,_,_)s", w_graphics_init);
  map_set(methods, "cactus" CLASS_NAME "fullscreens",         w_graphics_getFullscreen);
  map_set(methods, "cactus" CLASS_NAME "maxFpss",             w_graphics_getMaxFps);
  map_set(methods, "cactus" CLASS_NAME "setSize(_,_)s",       w_graphics_setSize);
  map_set(methods, "cactus" CLASS_NAME "fullscreen=(_)s",     w_graphics_setFullscreen);
  map_set(methods, "cactus" CLASS_NAME "maxFps=(_)s",         w_graphics_setMaxFps);
}
