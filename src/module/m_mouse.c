/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <SDL2/SDL.h>
#include "wren.h"

#define CLASS_NAME "Mouse"


extern SDL_Window *m_graphics_window;

static void w_mouse_setVisible(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_BOOL, "expected Bool");
  SDL_ShowCursor(wrenGetSlotBool(W, 1));
}


static void w_mouse_setPosition(WrenVM *W) {
	wrenCheckSlot(W, 1, WREN_TYPE_LIST, "expected List");
  wrenGetListElement(W, 1, 0, 2);
	wrenCheckSlot(W, 2, WREN_TYPE_NUM, "expected Num at index 0");
  wrenGetListElement(W, 1, 1, 3);
	wrenCheckSlot(W, 3, WREN_TYPE_NUM, "expected Num at index 1");
  SDL_WarpMouseInWindow(m_graphics_window,
    wrenGetSlotDouble(W, 2), wrenGetSlotDouble(W, 3)
  );
}


void wren_open_mouse(WrenVM *W) {
  WrenForeignMethodFn_Map *methods = &(wrenGetUserData(vm)->methods);
  map_set(methods, "cactus" CLASS_NAME "visible_=(_)s",  w_mouse_setVisible);
  map_set(methods, "cactus" CLASS_NAME "position_=(_)s", w_mouse_setPosition);
}
