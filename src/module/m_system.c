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


static void w_system_poll(WrenVM *W) {
  /* Create events table */
  wrenEnsureSlots(W, 5);
  wrenSetSlotNewList(W, 0);

  /* Handle events */
  int eventIdx = 1;
  SDL_Event e;
  while (SDL_PollEvent(&e)) {

    switch (e.type) {
      case SDL_QUIT:
        // luax_setfield_string(W, "type", "quit");
        break;

      case SDL_WINDOWEVENT:
        switch (e.window.event) {
          case SDL_WINDOWEVENT_RESIZED:
          // luax_setfield_string(W, "type", "resize");
          // luax_setfield_number(W, "width", e.window.data1);
          // luax_setfield_number(W, "height", e.window.data2);
          break;
        }
        break;

      case SDL_KEYDOWN:
        // luax_setfield_string(W, "type", "keydown");
        // luax_setfield_fstring(W, "key", "%s",
                              // SDL_GetKeyName(e.key.keysym.sym));
        break;

      case SDL_KEYUP:
        // luax_setfield_string(W, "type", "keyup");
        // luax_setfield_fstring(W, "key", "%s",
                              // SDL_GetKeyName(e.key.keysym.sym));
        break;

      case SDL_TEXTINPUT:
        // luax_setfield_string(W, "type", "textinput");
        // luax_setfield_fstring(W, "text", "%s", e.text.text);
        break;

      case SDL_MOUSEMOTION:
        // luax_setfield_string(W, "type", "mousemove");
        // luax_setfield_number(W, "x", e.motion.x);
        // luax_setfield_number(W, "y", e.motion.y);
        break;

      case SDL_MOUSEBUTTONDOWN:
        // luax_setfield_string(W, "type", "mousebuttondown");
        // luax_setfield_string(W, "button", buttonStr(e.button.button));
        // luax_setfield_number(W, "x", e.button.x);
        // luax_setfield_number(W, "y", e.button.y);
        break;

      case SDL_MOUSEBUTTONUP:
        // luax_setfield_string(W, "type", "mousebuttonup");
        // luax_setfield_string(W, "button", buttonStr(e.button.button));
        // luax_setfield_number(W, "x", e.button.x);
        // luax_setfield_number(W, "y", e.button.y);
        break;
    }

    /* Push event to events table */
    // lua_rawseti(W, -2, eventIdx++);
  }
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
  map_set(methods, "cactus" CLASS_NAME "info(_)s", w_system_info);
}
