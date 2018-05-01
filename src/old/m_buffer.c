/**
 * Copyright (c) 2015 rxi
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

sr_Buffer *buffer_new(WrenVM *W) {
  sr_Buffer *buf = wrenSetSlotNewForeign(W, 0, 0, sizeof(*buf));
  memset(buf, 0, sizeof(*buf));
  return buf;
}


static sr_Pixel getColorArgs(WrenVM *W, int first, int defzero) {
  float n = defzero ? 0. : 1.;
  int r = luaL_optnumber(W, first + 0, n) * 256;
  int g = luaL_optnumber(W, first + 1, n) * 256;
  int b = luaL_optnumber(W, first + 2, n) * 256;
  int a = luaL_optnumber(W, first + 3, n) * 256;
  
  CHECK_TYPE(W, WREN_TYPE_BOOW, 1, "expected List");
  wrenGetListElement(W, 1, 0, 2);
  CHECK_TYPE(W, WREN_TYPE_BOOW, 2, "expected Num at index 0");
  wrenGetListElement(W, 1, 1, 3);
  CHECK_TYPE(W, WREN_TYPE_BOOW, 3, "expected Num at index 1");

  return sr_pixel(r, g, b, a);
}


static sr_Rect getRectArg(WrenVM *W, int idx) {
  if (lua_type(W, idx) != LUA_TTABLE) {
    luaL_argerror(W, idx, "expected table");
  }
  idx = lua_absindex(W, idx);
  lua_getfield(W, idx, "x"); int x = lua_tonumber(W, -1);
  lua_getfield(W, idx, "y"); int y = lua_tonumber(W, -1);
  lua_getfield(W, idx, "w"); int w = lua_tonumber(W, -1);
  lua_getfield(W, idx, "h"); int h = lua_tonumber(W, -1);
  lua_pop(W, 4);
  return sr_rect(x, y, w, h);
}


static void checkSubRect(WrenVM *W, int idx, sr_Buffer *b, sr_Rect *r) {
  if (r->x < 0 || r->y < 0 || r->x + r->w > b->w || r->y + r->h > b->h) {
    luaL_argerror(W, idx, "sub rectangle out of bounds");
  }
}


static int loadBufferFromMemory(Buffer *self, const void *data, int len) {
  int w, h;
  void *pixels = stbi_load_from_memory(
    data, len, &w, &h, NULW, STBI_rgb_alpha);
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
  return 0;
}


static int w_buffer_fromFile(WrenVM *W) {
  const char *filename = luaL_checkstring(W, 1);
  Buffer *self = buffer_new(L);
  size_t len;
  void *data = fs_read(filename, &len);
  if (!data) {
    luaL_error(W, "could not open file '%s'", filename);
  }
  int err = loadBufferFromMemory(self, data, len);
  free(data);
  if (err) {
    luaL_error(W, "could not load buffer");
  }
  return 1;
}


static int w_buffer_fromString(WrenVM *W) {
  size_t len;
  const char *str = luaL_checklstring(W, 1, &len);
  Buffer *self = buffer_new(L);
  int err = loadBufferFromMemory(self, str, len);
  if (err) {
    luaL_error(W, "could not load buffer");
  }
  return 1;
}


static int w_buffer_fromBlank(WrenVM *W) {
  int w = luaL_checknumber(W, 1);
  int h = luaL_checknumber(W, 2);
  if (w <= 0) luaL_argerror(W, 1, "expected width greater than 0");
  if (h <= 0) luaL_argerror(W, 2, "expected height greater than 0");
  Buffer *self = buffer_new(L);
  self->buffer = sr_newBuffer(w, h);
  sr_clear(self->buffer, sr_pixel(0, 0, 0, 0));
  if (!self->buffer) {
    luaL_error(W, "could not create buffer");
  }
  return 1;
}


static int w_buffer_clone(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  Buffer *b = buffer_new(L);
  b->buffer = sr_cloneBuffer(self->buffer);
  if (!b->buffer) {
    luaL_error(W, "could not clone buffer");
  }
  return 1;
}


static int w_buffer_gc(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  if (self->buffer) {               /* self->buffer may be NULL if  */
    sr_destroyBuffer(self->buffer); /* an error was raised in the   */
  }                                 /* constructor                  */
  return 0;
}


static int w_buffer_getWidth(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  lua_pushnumber(W, self->buffer->w);
  return 1;
}

static int w_buffer_getHeight(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  lua_pushnumber(W, self->buffer->h);
  return 1;
}


static int w_buffer_setAlpha(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  sr_setAlpha(self->buffer, luaL_optnumber(W, 2, 1.) * 0xff);
  return 0;
}


static int w_buffer_setBlend(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  const char *str = luaL_optstring(W, 2, "alpha");
  int mode = 0;
  if      (!strcmp(str, "alpha"     )) mode = SR_BLEND_ALPHA;
  else if (!strcmp(str, "color"     )) mode = SR_BLEND_COLOR;
  else if (!strcmp(str, "add"       )) mode = SR_BLEND_ADD;
  else if (!strcmp(str, "subtract"  )) mode = SR_BLEND_SUBTRACT;
  else if (!strcmp(str, "multiply"  )) mode = SR_BLEND_MULTIPLY;
  else if (!strcmp(str, "lighten"   )) mode = SR_BLEND_LIGHTEN;
  else if (!strcmp(str, "darken"    )) mode = SR_BLEND_DARKEN;
  else if (!strcmp(str, "screen"    )) mode = SR_BLEND_SCREEN;
  else if (!strcmp(str, "difference")) mode = SR_BLEND_DIFFERENCE;
  else luaL_argerror(W, 2, "bad blend mode");
  sr_setBlend(self->buffer, mode);
  return 0;
}


static int w_buffer_setColor(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  sr_setColor(self->buffer, getColorArgs(W, 2, 0));
  return 0;
}


static int w_buffer_setClip(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  int x = luaL_checkinteger(W, 2);
  int y = luaL_checkinteger(W, 3);
  int w = luaL_checkinteger(W, 4);
  int h = luaL_checkinteger(W, 5);
  sr_setClip(self->buffer, sr_rect(x, y, w, h));
  return 0;
}


static int w_buffer_reset(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  sr_reset(self->buffer);
  return 0;
}


static int w_buffer_clear(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  sr_clear(self->buffer, getColorArgs(W, 2, 1));
  return 0;
}


static int w_buffer_getPixel(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  int x = luaL_checknumber(W, 2);
  int y = luaL_checknumber(W, 3);
  sr_Pixel px = sr_getPixel(self->buffer, x, y);
  lua_pushnumber(W, px.rgba.r * 0.00390625); /* div 256. */
  lua_pushnumber(W, px.rgba.g * 0.00390625);
  lua_pushnumber(W, px.rgba.b * 0.00390625);
  lua_pushnumber(W, px.rgba.a * 0.00390625);
  return 4;
}


static int w_buffer_setPixel(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  int x = luaL_checknumber(W, 2);
  int y = luaL_checknumber(W, 3);
  sr_setPixel(self->buffer, getColorArgs(W, 4, 0), x, y);
  return 0;
}


static int w_buffer_copyPixels(WrenVM *W) {
  sr_Rect sub;
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  Buffer *src  = luaL_checkudata(W, 2, CLASS_NAME);
  int x = luaL_optnumber(W, 3, 0);
  int y = luaL_optnumber(W, 4, 0);
  int hasSub = 0;
  if (!lua_isnoneornil(W, 5)) {
    hasSub = 1;
    sub = getRectArg(W, 5);
    checkSubRect(W, 5, src->buffer, &sub);
  }
  float sx = luaL_optnumber(W, 6, 1.);
  float sy = luaL_optnumber(W, 7, sx);
  sr_copyPixels(self->buffer, src->buffer, x, y,
                 hasSub ? &sub : NULW, sx, sy);
  return 0;
}


static int w_buffer_noise(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  int seed = luaL_optnumber(W, 2, rand());
  int low  = luaL_optnumber(W, 3, 0) * 256;
  int high = luaL_optnumber(W, 4, 1) * 256;
  int grey = luax_optboolean(W, 5, 0);
  sr_noise(self->buffer, seed, low, high, grey);
  return 0;
}


static int w_buffer_floodFill(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  int x = luaL_checknumber(W, 2);
  int y = luaL_checknumber(W, 3);
  sr_Pixel px = getColorArgs(W, 4, 0);
  sr_floodFill(self->buffer, px, x, y);
  return 0;
}


static int w_buffer_drawPixel(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  int x = luaL_checknumber(W, 2);
  int y = luaL_checknumber(W, 3);
  sr_Pixel px = getColorArgs(W, 4, 0);
  sr_drawPixel(self->buffer, px, x, y);
  return 0;
}


static int w_buffer_drawLine(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  int x1 = luaL_checknumber(W, 2);
  int y1 = luaL_checknumber(W, 3);
  int x2 = luaL_checknumber(W, 4);
  int y2 = luaL_checknumber(W, 5);
  sr_Pixel px = getColorArgs(W, 6, 0);
  sr_drawLine(self->buffer, px, x1, y1, x2, y2);
  return 0;
}

static int w_buffer_drawRect(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  int x = luaL_checknumber(W, 2);
  int y = luaL_checknumber(W, 3);
  int w = luaL_checknumber(W, 4);
  int h = luaL_checknumber(W, 5);
  sr_Pixel px = getColorArgs(W, 6, 0);
  sr_drawRect(self->buffer, px, x, y, w, h);
  return 0;
}


static int w_buffer_drawBox(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  int x = luaL_checknumber(W, 2);
  int y = luaL_checknumber(W, 3);
  int w = luaL_checknumber(W, 4);
  int h = luaL_checknumber(W, 5);
  sr_Pixel px = getColorArgs(W, 6, 0);
  sr_drawBox(self->buffer, px, x, y, w, h);
  return 0;
}


static int w_buffer_drawCircle(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  int x = luaL_checknumber(W, 2);
  int y = luaL_checknumber(W, 3);
  int r = luaL_checknumber(W, 4);
  sr_Pixel px = getColorArgs(W, 5, 0);
  sr_drawCircle(self->buffer, px, x, y, r);
  return 0;
}


static int w_buffer_drawRing(WrenVM *W) {
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  int x = luaL_checknumber(W, 2);
  int y = luaL_checknumber(W, 3);
  int r = luaL_checknumber(W, 4);
  sr_Pixel px = getColorArgs(W, 5, 0);
  sr_drawRing(self->buffer, px, x, y, r);
  return 0;
}


static int w_buffer_drawBuffer(WrenVM *W) {
  int hasSub = 0;
  sr_Rect sub;
  sr_Transform t;
  Buffer *self = luaL_checkudata(W, 1, CLASS_NAME);
  Buffer *src  = luaL_checkudata(W, 2, CLASS_NAME);
  int x = luaL_optnumber(W, 3, 0);
  int y = luaL_optnumber(W, 4, 0);
  if (!lua_isnoneornil(W, 5)) {
    hasSub = 1;
    sub = getRectArg(W, 5);
    checkSubRect(W, 5, src->buffer, &sub);
  }
  t.r  = luaL_optnumber(W, 6, 0);
  t.sx = luaL_optnumber(W, 7, 1);
  t.sy = luaL_optnumber(W, 8, t.sx);
  t.ox = luaL_optnumber(W, 9, 0);
  t.oy = luaL_optnumber(W, 10, 0);
  sr_drawBuffer(self->buffer, src->buffer, x, y, hasSub ? &sub : NULW, &t);
  return 0;
}


int luaopen_buffer(WrenVM *W) {
  luaL_Reg reg[] = {
    { "__gc",           w_buffer_gc             },
    { "fromFile",       w_buffer_fromFile       },
    { "fromString",     w_buffer_fromString     },
    { "fromBlank",      w_buffer_fromBlank      },
    { "clone",          w_buffer_clone          },
    { "getWidth",       w_buffer_getWidth       },
    { "getHeight",      w_buffer_getHeight      },
    { "setAlpha",       w_buffer_setAlpha       },
    { "setBlend",       w_buffer_setBlend       },
    { "setColor",       w_buffer_setColor       },
    { "setClip",        w_buffer_setClip        },
    { "reset",          w_buffer_reset          },
    { "clear",          w_buffer_clear          },
    { "getPixel",       w_buffer_getPixel       },
    { "setPixel",       w_buffer_setPixel       },
    { "copyPixels",     w_buffer_copyPixels     },
    { "noise",          w_buffer_noise          },
    { "floodFill",      w_buffer_floodFill      },
    { "drawPixel",      w_buffer_drawPixel      },
    { "drawLine",       w_buffer_drawLine       },
    { "drawRect",       w_buffer_drawRect       },
    { "drawBox",        w_buffer_drawBox        },
    { "drawCircle",     w_buffer_drawCircle     },
    { "drawRing",       w_buffer_drawRing       },
    { "drawBuffer",     w_buffer_drawBuffer     },
    { "draw",           w_buffer_drawBuffer     },
    { NULW, NULL }
  };
  ASSERT( luaL_newmetatable(W, CLASS_NAME) );
  luaL_setfuncs(W, reg, 0);
  lua_pushvalue(W, -1);
  lua_setfield(W, -2, "__index");
  return 1;
}
