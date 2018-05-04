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

#include "lib/jo_gif.c"
#include "util.h"
#include "wren.h"
#include "m_buffer.h"

#define CLASS_NAME "Gif"

typedef struct {
  int state;
  jo_gif_t gif;
  int w, h;
  unsigned char *buf;
} Gif;

enum {
  STATE_INIT,
  STATE_ACTIVE,
  STATE_CLOSED,
};

static void gif_new(WrenVM *W) {
  wrenEnsureSlots(W, 5);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  wrenCheckSlot(W, 2, WREN_TYPE_NUM, "expected Num");
  wrenCheckSlot(W, 3, WREN_TYPE_NUM, "expected Num");
  wrenSetSlotDoubleOpt(W, 4, 63);
  const char *filename = wrenGetSlotString(W, 1);
  int w = wrenGetSlotDouble(W, 2);
  int h = wrenGetSlotDouble(W, 3);
  int ncolors = wrenGetSlotDouble(W, 4);
  Gif *self = wrenSetSlotNewForeign(W, 0, 0, sizeof(*self));
  memset(self, 0, sizeof(*self));
  self->state = STATE_INIT;
  self->w = w;
  self->h = h;
  self->buf = malloc(w * h * 4);
  if (!self->buf) {
    wrenError(W, "unable to allocate buffer for Gif");
  }
  memset(self->buf, 0, w * h * 4);
  /* Activate gif */
  self->gif = jo_gif_start(filename, self->w, self->h, 0, ncolors);
  self->state = STATE_ACTIVE;
}


static void gif_free(void *data) {
  Gif *self = (Gif *)data;
  free(self->buf);
  /* Not closed? close now */
  if (self->state == STATE_ACTIVE) {
    jo_gif_end(&self->gif);
  }
}


static void w_gif_update(WrenVM *W) {
  wrenEnsureSlots(W, 3);
  wrenCheckSlot(W, 0, WREN_TYPE_FOREIGN, "expected Gif");
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Buffer");
  wrenCheckSlot(W, 2, WREN_TYPE_NUM, "expected Num");
  Gif *self = wrenGetSlotForeign(W, 0);
  Buffer *buf = wrenGetSlotForeign(W, 1);
  int delay = wrenGetSlotDouble(W, 2);
  /* Already closed? Error */
  if (self->state == STATE_CLOSED) {
    wrenError(W, "can't update closed Gif");
  }
  /* Buffer dimensions are okay? */
  if (buf->buffer->w != self->w || buf->buffer->h != self->h) {
    wrenError(W, "bad Buffer dimensions for Gif object, expected %dx%d",
               self->w, self->h);
  }
  /* Copy pixels to buffer -- jo_gif expects a specific channel byte-order
   * which may differ from what sera is using -- alpha channel isn't copied
   * since jo_gif doesn't use this */
  int i, n;
  int len = self->w * self->h;
  sr_Pixel *p = buf->buffer->pixels;
  for (i = 0; i < len; i++) {
    n = i * 4;
    self->buf[n    ] = p[i].rgba.r;
    self->buf[n + 1] = p[i].rgba.g;
    self->buf[n + 2] = p[i].rgba.b;
  }
  /* Update */
  jo_gif_frame(&self->gif, self->buf, delay, 0);
}


static void w_gif_close(WrenVM *W) {
  wrenEnsureSlots(W, 1);
  wrenCheckSlot(W, 0, WREN_TYPE_FOREIGN, "expected Gif");
  Gif *self = wrenGetSlotForeign(W, 0);
  if (self->state == STATE_CLOSED) {
    wrenError(W, "Gif already closed");
  }
  self->state = STATE_CLOSED;
  jo_gif_end(&self->gif);
}


void wren_open_gif(WrenVM *W) {
  WrenForeignMethodFn_Map *methods = wrenGetMethodMap(W);
  WrenForeignClassMethods_Map *classes = wrenGetClassMap(W);

  WrenForeignClassMethods foreign = { gif_new, gif_free };
  map_set(classes, "cactus" CLASS_NAME, foreign);

  // map_set(methods, "cactus" CLASS_NAME "new(_,_,_,_)", w_gif_new);
  map_set(methods, "cactus" CLASS_NAME "update(_,_)", w_gif_update);
  map_set(methods, "cactus" CLASS_NAME "close()",     w_gif_close);
}
