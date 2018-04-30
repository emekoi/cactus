/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <SDL2/SDL.h>
#include "wren.h"

#define MOUSE_CLASS_NAME "Mouse"

extern SDL_Window *m_graphics_window;

static void w_mouse_setVisible(WrenVM *W) {
  CHECK_TYPE(W, WREN_TYPE_BOOL, 1, "expected Bool");
  SDL_ShowCursor(wrenGetSlotBool(W, 1));
}


static void w_mouse_setPosition(WrenVM *W) {
	CHECK_TYPE(W, WREN_TYPE_BOOL, 1, "expected List");
  wrenGetListElement(W, 1, 0, 2);
	CHECK_TYPE(W, WREN_TYPE_BOOL, 2, "expected Num at index 0");
  wrenGetListElement(W, 1, 1, 3);
	CHECK_TYPE(W, WREN_TYPE_BOOL, 3, "expected Num at index 1");
  SDL_WarpMouseInWindow(m_graphics_window,
    wrenGetSlotDouble(W, 2), wrenGetSlotDouble(W, 3)
  );
}


void wren_open_mouse(WrenForeignMethodFn_Map *map) {
  map_set(map, "cactus" MOUSE_CLASS_NAME "visible__=(_)s",  w_mouse_setVisible);
  map_set(map, "cactus" MOUSE_CLASS_NAME "position__=(_)s", w_mouse_setPosition);
}
