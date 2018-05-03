/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */


#ifndef M_BUFFER_H
#define M_BUFFER_H

#include "wren.h"
#include "lib/sera/sera.h"

#define BUFFER_CLASS_NAME "Buffer"

typedef struct {
  sr_Buffer *buffer;
} Buffer;

void buffer_new(WrenVM *W);

#endif
