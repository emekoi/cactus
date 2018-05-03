/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"
#include "lib/sera/sera.h"
#include "m_buffer.h"
#include "util.h"
#include "fs.h"

#define CLASS_NAME BUFFER_CLASS_NAME

void buffer_new(WrenVM *W) {
  Buffer *buf = wrenSetSlotNewForeign(W, 0, 0, sizeof(*buf));
  memset(buf, 0, sizeof(*buf));
}

static void buffer_free(void *data) {
  Buffer *self = (Buffer *)data;
  if (self->buffer) {               /* self->buffer may be NULL if  */
    sr_destroyBuffer(self->buffer); /* an error was raised in the   */
  }                                 /* constructor                  */
}


static sr_Pixel getColorArgs(WrenVM *W, int first, bool defzero) {
  float n = defzero ? 0. : 1.;

  wrenSetSlotDoubleOpt(W, first + 0, n);
  wrenSetSlotDoubleOpt(W, first + 1, n);
  wrenSetSlotDoubleOpt(W, first + 2, n);
  wrenSetSlotDoubleOpt(W, first + 3, n);

  int r = wrenGetSlotDouble(W, first + 0) * 256;
  int g = wrenGetSlotDouble(W, first + 1) * 256;
  int b = wrenGetSlotDouble(W, first + 2) * 256;
  int a = wrenGetSlotDouble(W, first + 3) * 256;
  return sr_pixel(r, g, b, a);
}


static sr_Rect getRectArgs(WrenVM *W, int first) {
  wrenCheckSlot(W, first + 0, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, first + 1, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, first + 2, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, first + 3, WREN_TYPE_NUM, "expected Num");
  int x = wrenGetSlotDouble(W, first + 0);
  int y = wrenGetSlotDouble(W, first + 1);
  int w = wrenGetSlotDouble(W, first + 2);
  int h = wrenGetSlotDouble(W, first + 3);
  return sr_rect(x, y, w, h);
}


static void checkSubRect(WrenVM *W, sr_Buffer *b, sr_Rect *r) {
  if (r->x < 0 || r->y < 0 || r->x + r->w > b->w || r->y + r->h > b->h) {
    wrenError(W, "quad out of bounds");
  }
}


static int loadBufferFromMemory(Buffer *self, const void *data, int len) {
  int w, h;
  void *pixels = stbi_load_from_memory(
    data, len, &w, &h, NULL, STBI_rgb_alpha);
  if (!pixels) {
    return -1;
  }
  self->buffer = sr_newBuffer(w, h);
  if (!self->buffer) {
    free(pixels);
    return -1;
  }
  sr_loadPixels(self->buffer, pixels, SR_FMT_RGBA);
  free(pixels);
 }


static void w_buffer_fromFile(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  buffer_new(W);
  Buffer *self = wrenGetSlotForeign(W, 0);
  size_t len;
  void *data = fs_read(filename, &len);
  if (!data) {
    wrenError(W, "could not open file '%s'", filename);
  }
  int err = loadBufferFromMemory(self, data, len);
  free(data);
  if (err) {
    wrenError(W, "could not load buffer");
  }
 }


static void w_buffer_fromString(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  size_t len;
  const char *str = wrenGetSlotBytes(W, 1, &len);
  buffer_new(W);
  Buffer *self = wrenGetSlotForeign(W, 0);
  int err = loadBufferFromMemory(self, str, len);
  if (err) {
    wrenError(W, "could not load buffer");
  }
 }


static void w_buffer_fromBlank(WrenVM *W) {
  wrenEnsureSlots(W, 3);
  wrenCheckSlot(W, 1, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 2, WREN_TYPE_NUM, "expected Num");
  int w = wrenGetSlotDouble(W, 1);
  int h = wrenGetSlotDouble(W, 2);
  if (w <= 0) wrenError(W, "expected width greater than 0");
  if (h <= 0) wrenError(W, "expected height greater than 0");
  buffer_new(W);
  Buffer *self = wrenGetSlotForeign(W, 0);
  self->buffer = sr_newBuffer(w, h);
  sr_clear(self->buffer, sr_pixel(0, 0, 0, 0));
  if (!self->buffer) {
    wrenError(W, "could not create buffer");
  }
 }


static void w_buffer_clone(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  Buffer *self = wrenGetSlotForeign(W, 1);
  buffer_new(W);
  Buffer *b = wrenGetSlotForeign(W, 0);
  b->buffer = sr_cloneBuffer(self->buffer);
  if (!b->buffer) {
    wrenError(W, "could not clone buffer");
  }
 }


static void w_buffer_getWidth(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  Buffer *self = wrenGetSlotForeign(W, 1);
  wrenSetSlotDouble(W, 0, self->buffer->w);
 }

static void w_buffer_getHeight(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  Buffer *self = wrenGetSlotForeign(W, 1);
  wrenSetSlotDouble(W, 0, self->buffer->h);
 }


static void w_buffer_setAlpha(WrenVM *W) {
  wrenEnsureSlots(W, 3);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  wrenCheckSlot(W, 2, WREN_TYPE_FOREIGN, "expected Num");
  Buffer *self = wrenGetSlotForeign(W, 1);
  sr_setAlpha(self->buffer, wrenGetSlotDouble(W, 2) * 0xff);
 }


static void w_buffer_setBlend(WrenVM *W) {
  wrenEnsureSlots(W, 3);
  wrenSetSlotDoubleOpt(W, 2, SR_BLEND_ALPHA);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  Buffer *self = wrenGetSlotForeign(W, 1);
  int mode = wrenGetSlotDouble(W, 2);
  if (mode > SR_BLEND_DIFFERENCE) {
    wrenError(W, "bad blend mode");
  }
  sr_setBlend(self->buffer, mode);
 }


static void w_buffer_setColor(WrenVM *W) {
  wrenEnsureSlots(W, 2 + 4);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  Buffer *self = wrenGetSlotForeign(W, 1);
  sr_setColor(self->buffer, getColorArgs(W, 2, false));
 }


static void w_buffer_setClip(WrenVM *W) {
  wrenEnsureSlots(W, 2 + 4);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  Buffer *self = wrenGetSlotForeign(W, 1);
  sr_setClip(self->buffer, getRectArgs(W, 2));
 }


static void w_buffer_reset(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  Buffer *self = wrenGetSlotForeign(W, 1);
  sr_reset(self->buffer);
 }


static void w_buffer_clear(WrenVM *W) {
  wrenEnsureSlots(W, 2 + 4);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  Buffer *self = wrenGetSlotForeign(W, 1);
  sr_clear(self->buffer, getColorArgs(W, 2, true));
 }


static void w_buffer_getPixel(WrenVM *W) {
  wrenEnsureSlots(W, 4);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  wrenCheckSlot(W, 2, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 3, WREN_TYPE_NUM, "expected Num");
  Buffer *self = wrenGetSlotForeign(W, 1);
  int x = wrenGetSlotDouble(W, 2);
  int y = wrenGetSlotDouble(W, 3);
  sr_Pixel px = sr_getPixel(self->buffer, x, y);
  wrenSetSlotNewList(W, 0);
  wrenSetSlotDouble(W, 2, px.rgba.r / 256); /* div 256. */
  wrenInsertInList(W, 0, -1, 2);
  wrenSetSlotDouble(W, 2, px.rgba.g / 256);
  wrenInsertInList(W, 0, -1, 2);
  wrenSetSlotDouble(W, 2, px.rgba.b / 256);
  wrenInsertInList(W, 0, -1, 2);
  wrenSetSlotDouble(W, 2, px.rgba.a / 256);
  wrenInsertInList(W, 0, -1, 2);
 }


static void w_buffer_setPixel(WrenVM *W) {
  wrenEnsureSlots(W, 4 + 4);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  wrenCheckSlot(W, 2, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 3, WREN_TYPE_NUM, "expected Num");
  Buffer *self = wrenGetSlotForeign(W, 1);
  int x = wrenGetSlotDouble(W, 2);
  int y = wrenGetSlotDouble(W, 3);
  sr_setPixel(self->buffer, getColorArgs(W, 4, false), x, y);
 }


static void w_buffer_copyPixels(WrenVM *W) {
  wrenEnsureSlots(W, 5 + 4 + 2);
  wrenCheckSlot(W,  1, WREN_TYPE_FOREIGN, "expected Buffer");
  wrenCheckSlot(W,  2, WREN_TYPE_FOREIGN, "expected Buffer");
  wrenCheckSlot(W,  3, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W,  4, WREN_TYPE_NUM, "expected Num");
  wrenSetSlotDoubleOpt(W,  9, 1);
  wrenSetSlotDoubleOpt(W, 10, wrenGetSlotDouble(W, 9));

  Buffer *self = wrenGetSlotForeign(W, 1);
  Buffer *src  = wrenGetSlotForeign(W, 2);
  int x = wrenGetSlotDouble(W, 3);
  int y = wrenGetSlotDouble(W, 4);
  int hasSub = false;
  sr_Rect sub;
  if (wrenGetSlotType(W, 5) != WREN_TYPE_NULL) {
    hasSub = true;
    sub = getRectArgs(W, 5);
    checkSubRect(W, src->buffer, &sub);
  }
  float sx = wrenGetSlotDouble(W, 9);
  float sy = wrenGetSlotDouble(W, 10);
  sr_copyPixels(self->buffer, src->buffer, x, y, hasSub ? &sub : NULL, sx, sy);
 }


static void w_buffer_noise(WrenVM *W) {
  wrenEnsureSlots(W, 6);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  wrenSetSlotDoubleOpt(W, 2, rand());
  wrenSetSlotDoubleOpt(W, 3, 0);
  wrenSetSlotDoubleOpt(W, 4, 1);
  wrenSetSlotBoolOpt(W, 4, false);
  Buffer *self = wrenGetSlotForeign(W, 1);
  int seed = wrenGetSlotDouble(W, 2);
  int low  = wrenGetSlotDouble(W, 3) * 256;
  int high = wrenGetSlotDouble(W, 4) * 256;
  bool grey = wrenGetSlotBool(W, 5);
  sr_noise(self->buffer, seed, low, high, grey);
 }


static void w_buffer_floodFill(WrenVM *W) {
  wrenEnsureSlots(W, 4 + 4);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  wrenCheckSlot(W, 2, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 3, WREN_TYPE_NUM, "expected Num");
  Buffer *self = wrenGetSlotForeign(W, 1);
  int x = wrenGetSlotDouble(W, 2);
  int y = wrenGetSlotDouble(W, 3);
  sr_Pixel px = getColorArgs(W, 4, false);
  sr_floodFill(self->buffer, px, x, y);
 }


static void w_buffer_drawPixel(WrenVM *W) {
  wrenEnsureSlots(W, 4 + 4);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  wrenCheckSlot(W, 2, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 3, WREN_TYPE_NUM, "expected Num");
  Buffer *self = wrenGetSlotForeign(W, 1);
  int x = wrenGetSlotDouble(W, 2);
  int y = wrenGetSlotDouble(W, 3);
  sr_Pixel px = getColorArgs(W, 4, false);
  sr_drawPixel(self->buffer, px, x, y);
 }


static void w_buffer_drawLine(WrenVM *W) {
  wrenEnsureSlots(W, 6 + 4);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  wrenCheckSlot(W, 2, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 3, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 4, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 5, WREN_TYPE_NUM, "expected Num");
  Buffer *self = wrenGetSlotForeign(W, 1);
  int x1 = wrenGetSlotDouble(W, 2);
  int y1 = wrenGetSlotDouble(W, 3);
  int x2 = wrenGetSlotDouble(W, 4);
  int y2 = wrenGetSlotDouble(W, 5);
  sr_Pixel px = getColorArgs(W, 6, false);
  sr_drawLine(self->buffer, px, x1, y1, x2, y2);
 }

static void w_buffer_drawRect(WrenVM *W) {
  wrenEnsureSlots(W, 6 + 4);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  wrenCheckSlot(W, 2, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 3, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 4, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 5, WREN_TYPE_NUM, "expected Num");
  Buffer *self = wrenGetSlotForeign(W, 1);
  int x = wrenGetSlotDouble(W, 2);
  int y = wrenGetSlotDouble(W, 3);
  int w = wrenGetSlotDouble(W, 4);
  int h = wrenGetSlotDouble(W, 5);
  sr_Pixel px = getColorArgs(W, 6, false);
  sr_drawRect(self->buffer, px, x, y, w, h);
 }


static void w_buffer_drawBox(WrenVM *W) {
  wrenEnsureSlots(W, 6 + 4);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  wrenCheckSlot(W, 2, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 3, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 4, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 5, WREN_TYPE_NUM, "expected Num");
  Buffer *self = wrenGetSlotForeign(W, 1);
  int x = wrenGetSlotDouble(W, 2);
  int y = wrenGetSlotDouble(W, 3);
  int w = wrenGetSlotDouble(W, 4);
  int h = wrenGetSlotDouble(W, 5);
  sr_Pixel px = getColorArgs(W, 6, false);
  sr_drawBox(self->buffer, px, x, y, w, h);
 }


static void w_buffer_drawCircle(WrenVM *W) {
  wrenEnsureSlots(W, 5 + 4);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  wrenCheckSlot(W, 2, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 3, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 4, WREN_TYPE_NUM, "expected Num");
  Buffer *self = wrenGetSlotForeign(W, 1);
  int x = wrenGetSlotDouble(W, 2);
  int y = wrenGetSlotDouble(W, 3);
  int r = wrenGetSlotDouble(W, 4);
  sr_Pixel px = getColorArgs(W, 5, false);
  sr_drawCircle(self->buffer, px, x, y, r);
 }


static void w_buffer_drawRing(WrenVM *W) {
  wrenEnsureSlots(W, 5 + 4);
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  wrenCheckSlot(W, 2, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 3, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 4, WREN_TYPE_NUM, "expected Num");
  Buffer *self = wrenGetSlotForeign(W, 1);
  int x = wrenGetSlotDouble(W, 2);
  int y = wrenGetSlotDouble(W, 3);
  int r = wrenGetSlotDouble(W, 4);
  sr_Pixel px = getColorArgs(W, 5, false);
  sr_drawRing(self->buffer, px, x, y, r);
 }


static void w_buffer_drawBuffer(WrenVM *W) {
  wrenEnsureSlots(W, 6 + 4 + 5);
  wrenCheckSlot(W,  1, WREN_TYPE_FOREIGN, "expected Buffer");
  wrenCheckSlot(W,  2, WREN_TYPE_FOREIGN, "expected Buffer");

  wrenCheckSlot(W,  3, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W,  4, WREN_TYPE_NUM, "expected Num");

  wrenSetSlotDoubleOpt(W,   9, 0);
  wrenSetSlotDoubleOpt(W,  10, 0);
  wrenSetSlotDoubleOpt(W,  11, 0);
  wrenSetSlotDoubleOpt(W,  12, 1);
  wrenSetSlotDoubleOpt(W,  13, wrenGetSlotDouble(W, 12));

  int hasSub = 0;
  sr_Rect sub;
  sr_Transform t;
  Buffer *self = wrenGetSlotForeign(W, 1);
  Buffer *src  = wrenGetSlotForeign(W, 2);
  int x = wrenGetSlotDouble(W, 3);
  int y = wrenGetSlotDouble(W, 4);
  if (wrenGetSlotType(W, 5) != WREN_TYPE_NULL) {
    hasSub = 1;
    sub = getRectArgs(W, 5);
    checkSubRect(W, src->buffer, &sub);
  }
  t.ox = wrenGetSlotDouble(W,  9);
  t.oy = wrenGetSlotDouble(W, 10);
  t.r  = wrenGetSlotDouble(W, 11);
  t.sx = wrenGetSlotDouble(W, 12);
  t.sy = wrenGetSlotDouble(W, 13);
  sr_drawBuffer(self->buffer, src->buffer, x, y, hasSub ? &sub : NULL, &t);
 }


void wren_open_buffer(WrenVM *W) {
  WrenForeignMethodFn_Map *methods = wrenGetMethodMap(W);
  WrenForeignClassMethods_Map *classes = wrenGetClassMap(W);

  WrenForeignClassMethods foreign = { buffer_new, buffer_free };
  map_set(classes, "cactus" CLASS_NAME, foreign);

  map_set(methods, "cactus" CLASS_NAME "fromFile(_)s",                   w_buffer_fromFile);
  map_set(methods, "cactus" CLASS_NAME "fromString(_)s",                 w_buffer_fromString);
  map_set(methods, "cactus" CLASS_NAME "fromBlank(_,_)s",                w_buffer_fromBlank);
  map_set(methods, "cactus" CLASS_NAME "w",                              w_buffer_getWidth);
  map_set(methods, "cactus" CLASS_NAME "h",                              w_buffer_getHeight);
  map_set(methods, "cactus" CLASS_NAME "setAlpha_(_)",                   w_buffer_setAlpha);
  map_set(methods, "cactus" CLASS_NAME "setBlend_(_)",                   w_buffer_setBlend);
  map_set(methods, "cactus" CLASS_NAME "setColor_(_,_,_,_)",             w_buffer_setColor);
  map_set(methods, "cactus" CLASS_NAME "setClip_(_,_,_,_)",              w_buffer_setClip);
  map_set(methods, "cactus" CLASS_NAME "clone()",                        w_buffer_clone);
  map_set(methods, "cactus" CLASS_NAME "reset()",                        w_buffer_reset);
  map_set(methods, "cactus" CLASS_NAME "noise(_,_,_,_)",                 w_buffer_noise);
  map_set(methods, "cactus" CLASS_NAME "clear_(_,_,_,_)",                w_buffer_clear);
  map_set(methods, "cactus" CLASS_NAME "getPixel_(_,_)",                 w_buffer_getPixel);
  map_set(methods, "cactus" CLASS_NAME "setPixel_(_,_,_,_,_,_)",         w_buffer_setPixel);
  map_set(methods, "cactus" CLASS_NAME "copyPixels_(_,_,_,_,_,_,_,_,_)", w_buffer_copyPixels);
  map_set(methods, "cactus" CLASS_NAME "floodFill_(_,_,_,_,_,_)",        w_buffer_floodFill);
  map_set(methods, "cactus" CLASS_NAME "drawPixel_(_,_,_,_,_,_)",        w_buffer_drawPixel);
  map_set(methods, "cactus" CLASS_NAME "drawLine_(_,_,_,_,_,_,_,_)",     w_buffer_drawLine);
  map_set(methods, "cactus" CLASS_NAME "drawRect_(_,_,_,_,_,_,_,_)",     w_buffer_drawRect);
  map_set(methods, "cactus" CLASS_NAME "drawBox_(_,_,_,_,_,_,_,_)",      w_buffer_drawBox);
  map_set(methods, "cactus" CLASS_NAME "drawCircle_(_,_,_,_,_,_,_)",     w_buffer_drawCircle);
  map_set(methods, "cactus" CLASS_NAME "drawRing_(_,_,_,_,_,_,_)",       w_buffer_drawRing);
  map_set(methods, "cactus" CLASS_NAME "draw_(_,_,_,_,_,_,_,_,_,_,_,_)", w_buffer_drawBuffer);
}
