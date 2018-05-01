/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <stdlib.h>
#include "wren.h"
#include "fs.h"

#define CLASS_NAME "FS"


static void checkError(WrenVM *W, int err, const char *str) {
  if (!err) return;
  if (err == FS_ENOWRITEPATH || !str) {
    wrenError(W, "%s", fs_errorStr(err));
  }
  wrenError(W, "%s '%s'", fs_errorStr(err), str);
}


static void w_fs_mount(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *path = wrenGetSlotString(W, 1);
  int res = fs_mount(path);
  checkError(W, res, path);
  wrenEnsureSlots(W, 1);
  wrenSetSlotBool(W, 0, true);
}


static void w_fs_unmount(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *path = wrenGetSlotString(W, 1);
  int res = fs_unmount(path);
  checkError(W, res, path);
  wrenEnsureSlots(W, 1);
  wrenSetSlotBool(W, 0, true);
}


static void w_fs_setWritePath(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *path = wrenGetSlotString(W, 1);
  int res = fs_setWritePath(path);
  checkError(W, res, path);
  wrenEnsureSlots(W, 1);
  wrenSetSlotBool(W, 0, true);
}


static void w_fs_exists(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  wrenEnsureSlots(W, 1);
  wrenSetSlotBool(W, 0, fs_exists(filename));
}


static void w_fs_getSize(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  size_t sz;
  int res = fs_size(filename, &sz);
  checkError(W, res, filename);
  wrenEnsureSlots(W, 1);
  wrenSetSlotDouble(W, 0, sz);
}


static void w_fs_getModified(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  unsigned t;
  int res = fs_modified(filename, &t);
  checkError(W, res, filename);
  wrenEnsureSlots(W, 1);
  wrenSetSlotDouble(W, 0, t);
}


static void w_fs_read(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  size_t len;
  char *data = fs_read(filename, &len);
  if (!data) {
    wrenError(W, "could not read file '%s'", filename);
  }
  wrenEnsureSlots(W, 1);
  wrenSetSlotBytes(W, 0, data, len);
  free(data);
}


static void w_fs_isDir(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  wrenEnsureSlots(W, 1);
  wrenSetSlotBool(W, 0, fs_isDir(filename));
}


static void w_fs_listDir(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *path = wrenGetSlotString(W, 1);
  fs_FileListNode *list = fs_listDir(path);
  wrenEnsureSlots(W, 2);
  wrenSetSlotNewList(W, 0);
  int i = 1;
  fs_FileListNode *n = list;
  for (int i = 0; n; i++) {
    wrenSetSlotString(W, 1, n->name);
    wrenInsertInList(W, 0, i, 1);
    n = n->next;
  }
  fs_freeFileList(list);
}


static void w_fs_write(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  wrenCheckSlot(W, 2, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  size_t len;
  const char *data = wrenGetSlotBytes(W, 2, &len);
  int res = fs_write(filename, data, len);
  checkError(W, res, filename);
  wrenEnsureSlots(W, 1);
  wrenSetSlotBool(W, 0, true);
}


static void w_fs_append(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  wrenCheckSlot(W, 2, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  size_t len;
  const char *data = wrenGetSlotBytes(W, 2, &len);
  int res = fs_append(filename, data, len);
  checkError(W, res, filename);
  wrenEnsureSlots(W, 1);
  wrenSetSlotBool(W, 0, true);
}


static void w_fs_delete(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *filename = wrenGetSlotString(W, 1);
  int res = fs_delete(filename);
  checkError(W, res, filename);
  wrenEnsureSlots(W, 1);
  wrenSetSlotBool(W, 0, true);
}


static void w_fs_makeDirs(WrenVM *W) {
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *path = wrenGetSlotString(W, 1);
  int res = fs_makeDirs(path);
  checkError(W, res, path);
  wrenEnsureSlots(W, 1);
  wrenSetSlotBool(W, 0, true);
}


void wren_open_fs(WrenVM *W) {
  WrenForeignMethodFn_Map *methods = &(wrenGetUserData(vm)->methods);
  map_set(methods, "cactus" CLASS_NAME "mount(_)s",        w_fs_mount);
  map_set(methods, "cactus" CLASS_NAME "unmount(_)s",      w_fs_unmount);
  map_set(methods, "cactus" CLASS_NAME "setWritePath(_)s", w_fs_setWritePath);
  map_set(methods, "cactus" CLASS_NAME "exists(_)s",       w_fs_exists);
  map_set(methods, "cactus" CLASS_NAME "getSize(_)s",      w_fs_getSize);
  map_set(methods, "cactus" CLASS_NAME "getModified(_)s",  w_fs_getModified);
  map_set(methods, "cactus" CLASS_NAME "read(_)s",         w_fs_read);
  map_set(methods, "cactus" CLASS_NAME "isDir(_)s",        w_fs_isDir);
  map_set(methods, "cactus" CLASS_NAME "listDir(_)s",      w_fs_listDir);
  map_set(methods, "cactus" CLASS_NAME "write(_,_)s",      w_fs_write);
  map_set(methods, "cactus" CLASS_NAME "append(_,_)s",     w_fs_append);
  map_set(methods, "cactus" CLASS_NAME "delete(_)s",       w_fs_delete);
  map_set(methods, "cactus" CLASS_NAME "makeDirs(_)s",     w_fs_makeDirs);
  atexit(fs_deinit);
 }
