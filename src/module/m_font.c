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
#include "wren.h"
#include "fs.h"
#include "ttf.h"
#include "m_buffer.h"

#define CLASS_NAME "Font"
#define DEFAULT_FONTSIZE 16

typedef struct {
  ttf_Font *font;
} Font;


static const char *loadFontFromMemory(
  Font *self, const void *data, int len, int ptsize
) {
  self->font = ttf_new(data, len);
  if (!self->font) {
    return "could not load font";
  }
  ttf_ptsize(self->font, ptsize);
  return NULL;
}


static void font_new(WrenVM *W) {
  wrenSetSlotDoubleOpt(W, 3, DEFAULT_FONTSIZE);
  Font *self = wrenSetSlotNewForeign(W, 0, 0, sizeof(*self));
  memset(self, 0, sizeof(*self));
  int len = 0;
  void *data = (void *)wrenGetSlotBytes(W, 1, &len);
  int ptsize = wrenGetSlotDouble(W, 2);
  const char *err = loadFontFromMemory(self, data, len, ptsize);
  if (err) wrenError(W, "%s", err);
}


static void font_free(void *data) {
  Font *self = (Font*)data;
  if (self->font) {
    ttf_destroy(self->font);
  }
 }


static void w_font_fromFile(WrenVM *W) {
  wrenEnsureSlots(W, 3);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  size_t len;
  void *data = fs_read(filename, &len);
  /* Load new font */
  if (!data) {
    wrenError(W, "could not open file '%s'", filename);
  }
  wrenSetSlotBytes(W, 1, data, len);
  free(data);
 }


static void w_font_fromString(WrenVM *W) {
  wrenEnsureSlots(W, 3);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  font_new(W);
 }


static void w_font_fromEmbedded(WrenVM *W) {
  #include "default_ttf.h"
  wrenEnsureSlots(W, 3);
  // wrenSetSlotBytes(W, 1, default_ttf, sizeof(default_ttf));
  wrenSetSlotString(W, 1, default_ttf);
  font_new(W);
 }


static void w_font_render(WrenVM *W) {
  wrenEnsureSlots(W, 3);
  wrenCheckSlot(W, 0, WREN_TYPE_FOREIGN, "expected Font");
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  int w, h;
  Font *self = wrenGetSlotForeign(W, 0);
  const char *str = wrenGetSlotString(W, 1);
  if (!str || *str == '\0') str = " ";
  void *data = ttf_render(self->font, str, &w, &h);
  if (!data) {
    wrenError(W, "could not render text");
  }
  /* Load bitmap and free intermediate 8bit bitmap */
  wrenSetSlotDouble(W, 1, w);
  wrenSetSlotDouble(W, 2, h);
  wrenGetVariable(W, "main", "Buffer", 0);
  buffer_new(W);
  Buffer *b = wrenGetSlotForeign(W, 0);
  b->buffer = sr_newBuffer(w, h);
  if (!b->buffer) {
    free(data);
    wrenError(W, "could not create buffer");
  }
  sr_loadPixels8(b->buffer, data, NULL);
  free(data);
 }


static void w_font_getWidth(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 0, WREN_TYPE_FOREIGN, "expected Font");
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  Font *self = wrenGetSlotForeign(W, 0);
  const char *str = wrenGetSlotString(W, 1);
  wrenSetSlotDouble(W, 0, ttf_width(self->font, str));
 }


static void w_font_getHeight(WrenVM *W) {
  wrenEnsureSlots(W, 1);
  wrenCheckSlot(W, 0, WREN_TYPE_FOREIGN, "expected Font");
  Font *self = wrenGetSlotForeign(W, 0);
  wrenSetSlotDouble(W, 0, ttf_height(self->font));
 }


static void w_font_getSize(WrenVM *W) {
  wrenEnsureSlots(W, 1);
  wrenCheckSlot(W, 0, WREN_TYPE_FOREIGN, "expected Font");
  Font *self = wrenGetSlotForeign(W, 0);
  wrenSetSlotDouble(W, 0, self->font->ptsize);
 }


void wren_open_font(WrenVM *W) {
  WrenForeignMethodFn_Map *methods = wrenGetMethodMap(W);
  WrenForeignClassMethods_Map *classes = wrenGetClassMap(W);

  WrenForeignClassMethods foreign = { font_new, font_free };
  map_set(classes, "cactus" CLASS_NAME, foreign);

  map_set(methods, "cactus" CLASS_NAME "fromFile(_,_)s",   w_font_fromFile);
  map_set(methods, "cactus" CLASS_NAME "fromString(_,_)s", w_font_fromString);
  map_set(methods, "cactus" CLASS_NAME "fromEmbedded(_)s", w_font_fromEmbedded);
  map_set(methods, "cactus" CLASS_NAME "height",           w_font_getHeight);
  map_set(methods, "cactus" CLASS_NAME "size",             w_font_getSize);
  map_set(methods, "cactus" CLASS_NAME "getWidth(_)",      w_font_getWidth);
  map_set(methods, "cactus" CLASS_NAME "render(_)",        w_font_render);


 }
