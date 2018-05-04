/**
 * Copyright (c) 2015 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#include <SDL2/SDL.h>
#include "util.h"
#include "wren.h"

#if _WIN32
  #include <windows.h>
#elif __linux__
  #include <unistd.h>
#elif __APPLE__
  #include <mach-o/dyld.h>
#endif

#define CLASS_NAME "Cactus"


static const char *buttonStr(int id) {
  switch (id) {
    case SDL_BUTTON_LEFT   : return "left";
    case SDL_BUTTON_MIDDLE : return "middle";
    case SDL_BUTTON_RIGHT  : return "right";
    case SDL_BUTTON_X1     : return "wheelup";
    case SDL_BUTTON_X2     : return "wheeldown";
    default : return "?";
  }
}


#define SLOT(i) i + 1

static void w_system_poll(WrenVM *W) {
  /* Create events table */
  wrenEnsureSlots(W, 6);
  wrenSetSlotNewList(W, 0);

  /* Handle events */
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    int listLen = 0;
    wrenSetSlotNewList(W, 1);

    switch (e.type) {
      case SDL_QUIT:
        listLen += 1;
        wrenSetSlotString(W, SLOT(1), "quit");
        break;

      case SDL_WINDOWEVENT:
        switch (e.window.event) {
          case SDL_WINDOWEVENT_RESIZED:
            listLen += 3;
            wrenSetSlotString(W, SLOT(1), "resize");
            wrenSetSlotDouble(W, SLOT(2), e.window.data1);
            wrenSetSlotDouble(W, SLOT(3), e.window.data2);
          break;
        }
        break;

      case SDL_KEYDOWN:
        listLen += 2;
        wrenSetSlotString(W, SLOT(1), "keydown");
        wrenSetSlotStringFormat(W, SLOT(2), "%s", SDL_GetKeyName(e.key.keysym.sym));
        break;

      case SDL_KEYUP:
        listLen += 2;
        wrenSetSlotString(W, SLOT(1), "keyup");
        wrenSetSlotStringFormat(W, SLOT(2), "%s", SDL_GetKeyName(e.key.keysym.sym));
        break;

      case SDL_TEXTINPUT:
        listLen += 2;
        wrenSetSlotString(W, SLOT(1), "textinput");
        wrenSetSlotStringFormat(W, SLOT(2), "%s", e.text.text);
        break;

      case SDL_MOUSEMOTION:
        listLen += 3;
        wrenSetSlotString(W, SLOT(1), "mousemove");
        wrenSetSlotDouble(W, SLOT(2), e.motion.x);
        wrenSetSlotDouble(W, SLOT(3), e.motion.y);
        break;

      case SDL_MOUSEBUTTONDOWN:
        listLen += 4;
        wrenSetSlotString(W, SLOT(1), "mousebuttondown");
        wrenSetSlotString(W, SLOT(2), buttonStr(e.button.button));
        wrenSetSlotDouble(W, SLOT(3), e.button.x);
        wrenSetSlotDouble(W, SLOT(4), e.button.y);
        break;

      case SDL_MOUSEBUTTONUP:
        listLen += 4;
        wrenSetSlotString(W, SLOT(1), "mousebuttonup");
        wrenSetSlotString(W, SLOT(2), buttonStr(e.button.button));
        wrenSetSlotDouble(W, SLOT(3), e.button.x);
        wrenSetSlotDouble(W, SLOT(4), e.button.y);
        break;
    }

    /* Push event to list */
    for (int i = 0; i < listLen; i++) {
      wrenInsertInList(W, 1, i, SLOT(i + 1));
    }

    if (wrenGetListCount(W, 1) > 0) {
      wrenInsertInList(W, 0, -1, 1);
    }
  }
}

#undef SLOT


static void w_system_exit(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  int code = wrenGetSlotDouble(W, 1);
  exit(code);
}


static char *dirname(char *str) {
  char *p = str + strlen(str);
  while (p != str) {
    if (*p == '/' || *p == '\\') {
      *p = '\0';
      break;
    }
    p--;
  }
  return str;
}

static void w_system_info(WrenVM *W) {
  wrenEnsureSlots(W, 2);
  wrenCheckSlot(W, 1, WREN_TYPE_STRING, "expected String");
  const char *str = wrenGetSlotString(W, 1);
  if (!strcmp(str, "os")) {
#if _WIN32
    wrenSetSlotString(W, 0, "windows");
#elif __linux__
    wrenSetSlotString(W, 0, "linux");
#elif __FreeBSD__
    wrenSetSlotString(W, 0, "bsd");
#elif __APPLE__
    wrenSetSlotString(W, 0, "osx");
#else
    wrenSetSlotString(W, 0, "?");
#endif

  } else if (!strcmp(str, "exedir")) {
    UNUSED(dirname);
#if _WIN32
    char buf[1024];
    int len = GetModuleFileName(NULL, buf, sizeof(buf) - 1);
    buf[len] = '\0';
    dirname(buf);
    wrenSetSlotStringFormat(W, 0, "%s", buf);
#elif __linux__
    char path[128];
    char buf[1024];
    sprintf(path, "/proc/%d/exe", getpid());
    int len = readlink(path, buf, sizeof(buf) - 1);
    ASSERT( len != -1 );
    buf[len] = '\0';
    dirname(buf);
    wrenSetSlotStringFormat(W, 0, "%s", buf);
#elif __FreeBSD__
    /* TODO : Implement this */
    wrenSetSlotString(W, 0, ".");
#elif __APPLE__
    char buf[1024];
    uint32_t size = sizeof(buf);
    ASSERT( _NSGetExecutablePath(buf, &size) == 0 );
    dirname(buf);
    wrenSetSlotStringFormat(W, 0, "%s", buf);
#else
    wrenSetSlotString(W, 0, ".");
#endif

  } else if (!strcmp(str, "appdata")) {
#if _WIN32
    wrenSetSlotStringFormat(W, 0, "%s", getenv("APPDATA"));
#elif __APPLE__
    wrenSetSlotStringFormat(W, 0, "%s/Library/Application Support", getenv("HOME"));
#else
    wrenSetSlotStringFormat(W, 0, "%s/.local/share", getenv("HOME"));
#endif
} else {
    wrenError(W, "invalid argument");
  }
}


void wren_open_system(WrenVM *W) {
  WrenForeignMethodFn_Map *methods = wrenGetMethodMap(W);

  map_set(methods, "cactus" CLASS_NAME "poll()s",  w_system_poll);
  map_set(methods, "cactus" CLASS_NAME "exit(_)s", w_system_exit);
  map_set(methods, "cactus" CLASS_NAME "info(_)s", w_system_info);
}
