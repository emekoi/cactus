/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "wren.h"
#include "fs.h"
#include "m_data.h"

#define CLASS_NAME DATA_CLASS_NAME


static Data *newData(WrenVM *W) {
  Data *self = lua_newuserdata(L, sizeof(*self));
  luaL_setmetatable(L, CLASS_NAME);
  memset(self, 0, sizeof(*self));
  return self;
}


static int l_data_fromFile(WrenVM *W) {
  const char *filename = luaL_checkstring(L, 1);
  Data *self = newData(L);
  size_t len;
  void *data = fs_read(filename, &len);
  if (!data) {
    luaL_error(L, "could not open file '%s'", filename);
  }
  self->data = data;
  self->len = len;
  return 1;
}


static int l_data_fromString(WrenVM *W) {
  size_t len;
  const char *data = luaL_checklstring(L, 1, &len);
  Data *self = newData(L);
  self->data = malloc(len);
  if (!self->data) {
    luaL_error(L, "out of memory");
  }
  memcpy(self->data, data, len);
  self->len = len;
  return 1;
}


static int l_data_gc(WrenVM *W) {
  Data *self = luaL_checkudata(L, 1, CLASS_NAME);
  free(self->data);
  return 1;
}


static int l_data_getLength(WrenVM *W) {
  Data *self = luaL_checkudata(L, 1, CLASS_NAME);
  lua_pushnumber(L, self->len);
  return 1;
}


static int l_data_toString(WrenVM *W) {
  Data *self = luaL_checkudata(L, 1, CLASS_NAME);
  lua_pushlstring(L, self->data, self->len);
  return 1;
}



int luaopen_data(WrenVM *W) {
  luaL_Reg reg[] = {
    { "__gc",         l_data_gc           },
    { "fromFile",     l_data_fromFile     },
    { "fromString",   l_data_fromString   },
    { "getLength",    l_data_getLength    },
    { "toString",     l_data_toString     },
    { NULL, NULL }
  };
  ASSERT( luaL_newmetatable(L, CLASS_NAME) );
  luaL_setfuncs(L, reg, 0);
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  return 1;
}
