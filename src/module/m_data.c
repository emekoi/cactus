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

#define CLASS_NAME "Data"


static void *newData(WrenVM *W) {
  Data *self = wrenSetSlotNewForeign(W, 0, 0, sizeof(*self));
  memset(self, 0, sizeof(*self));
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  self->data = wrenGetSlotBytes(W, 1, &self->len);
}


static void freeData(WrenVM *W) {
  Data *self = wrenGetSlotForeign(W, 1);
  free(self->data);
}


static void w_data_fromFile(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  wrenEnsureSlots(W, 1);

  size_t len;
  void *data = fs_read(filename, &len);

  if (!data) {
    wrenError(W, "could not open file '%s'", filename);
  }

  wrenSetSlotBytes(W, 1, data, len);
  free(data);
  newData(W);
}


static void w_data_fromString(WrenVM *W) {
  wrenEnsureSlots(W, 1);
  newData(W);
}


static void w_data_getLength(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Data");
  Data *self = wrenGetSlotForeign(W, 1);
  wrenEnsureSlots(W, 1);
  wrenSetSlotDouble(W, 0, self->len);
}


static void w_data_toString(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_FOREIGN, "expected Data");
  Data *self = wrenGetSlotForeign(W, 1);
  wrenEnsureSlots(W, 1);
  wrenSetSlotBytes(W, 0, self->data, self->len);
}


void wren_open_data(WrenVM *W) {
  WrenForeignMethodFn_Map *methods = &(wrenGetUserData(vm)->methods);
  WrenForeignClassMethods_Map *classes = &(wrenGetUserData(vm)->classes);

  map_set(classes, "cactus" CLASS_NAME, { newData, freeData });

  map_set(methods, "cactus" CLASS_NAME "fromFile(_)s",   w_data_fromFile);
  map_set(methods, "cactus" CLASS_NAME "fromString(_)s", w_data_fromString);
  map_set(methods, "cactus" CLASS_NAME "length",         w_data_getLength);
  map_set(methods, "cactus" CLASS_NAME "toString()",     w_data_toString);
}
