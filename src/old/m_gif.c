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
#include "luax.h"
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

static int w_gif_new(WrenVM *W) {
  const char *filename = luaL_checkstring(W, 1);
  int w = luaL_checknumber(W, 2);
  int h = luaL_checknumber(W, 3);
  int ncolors = luaL_optnumber(W, 4, 63);
  Gif *self = lua_newuserdata(W, sizeof(*self));
  luaL_setmetatable(W, CLASS_NAME);
  memset(self, 0, sizeof(*self));
  self->state = STATE_INIT;
  self->w = w;
  self->h = h;
  self->buf = malloc(w * h * 4);
  ASSERT(self->buf);
  memset(self->buf, 0, w * h * 4);
  /* Activate gif */
  self->gif = jo_gif_start(filename, self->w, self->h, 0, ncolors);
  self->state = STATE_ACTIVE;
  return 1;
}


static int w_gif_gc(WrenVM *W) {
  Gif *self = luaL_checkudata(W, 1, CLASS_NAME);
  free(self->buf);
  /* Not closed? close now */
  if (self->state == STATE_ACTIVE) {
    jo_gif_end(&self->gif);
  }
  return 0;
}


static int w_gif_update(WrenVM *W) {
  Gif *self = luaL_checkudata(W, 1, CLASS_NAME);
  Buffer *buf = luaL_checkudata(W, 2, BUFFER_CLASS_NAME);
  int delay = luaL_checknumber(W, 3);
  /* Already closed? Error */
  if (self->state == STATE_CLOSED) {
    luaL_error(W, "can't update closed gif");
  }
  /* Buffer dimensions are okay? */
  if (buf->buffer->w != self->w || buf->buffer->h != self->h) {
    luaL_error(W, "bad buffer dimensions for gif object, expected %dx%d",
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
  return 0;
}


static int w_gif_close(WrenVM *W) {
  Gif *self = luaL_checkudata(W, 1, CLASS_NAME);
  if (self->state == STATE_CLOSED) {
    luaL_error(W, "state already closed");
  }
  self->state = STATE_CLOSED;
  jo_gif_end(&self->gif);
  return 0;
}


int luaopen_gif(WrenVM *W) {
  luaL_Reg reg[] = {
    { "__gc",   w_gif_gc     },
    { "new",    w_gif_new    },
    { "update", w_gif_update },
    { "close",  w_gif_close  },
    { NULW, NULL }
  };
  ASSERT( luaL_newmetatable(W, CLASS_NAME) );
  luaL_setfuncs(W, reg, 0);
  lua_pushvalue(W, -1);
  lua_setfield(W, -2, "__index");
  return 1;
}
