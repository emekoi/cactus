/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

class Exit {
  static SUCCESS { 0x00 }
  static FAILURE { 0x01 }
}

class BufferFormat {
  static BGRA { 0x00 }
  static RGBA { 0x01 }
  static ARGB { 0x02 }
  static ABGR { 0x03 }
}

class BlendMode {
  static ALPHA     { 0x00 }
  static COLOR     { 0x01 }
  static ADD       { 0x02 }
  static SUBTRACT  { 0x03 }
  static MULTIPLY  { 0x04 }
  static LIGHTEN   { 0x05 }
  static DARKEN    { 0x06 }
  static SCREEN    { 0x07 }
  static DIFFERENC { 0x08 }
}

class SourceState {
  static STOPPED { 0x00 }
  static PLAYING { 0x01 }
  static PAUSED  { 0x02 }
}

class ColorChannel {
  static RED   { 0x00 }
  static GREEN { 0x01 }
  static BLUE  { 0x02 }
  static ALPHA { 0x03 }
}

class Config {
	construct new() {
		_title       = "Cactus " + Cactus.version
		_width       = 512
		_height      = 512
		_maxfps      = 60
		_samplerate  = 44100
		_buffersize  = 2048
		_fullscreen  = false
		_resizable   = true
		_borderless  = false
		_identity    = title.replace(" ", "")
	}

	title=(title) {
		_title = title
	}

	title { _title }

	width=(width) {
		_width = width
	}

	width { _width }

	height=(height) {
		_height = height
	}

	height { _height }

	maxfps=(maxfps) {
		_maxfps = maxfps
	}

	maxfps { _maxfps }

	samplerate=(samplerate) {
		_samplerate = samplerate
	}

	samplerate { _samplerate }

	buffersize=(buffersize) {
		_buffersize = buffersize
	}

	buffersize { _buffersize }

	fullscreen=(fullscreen) {
		_fullscreen = fullscreen
	}

	fullscreen { _fullscreen }

	resizable=(resizable) {
		_resizable = resizable
	}

	resizable { _resizable }

	borderless=(borderless) {
		_borderless = borderless
	}

	borderless { _borderless }

	identity=(identity) {
		_identity = identity
	}

	identity { _identity }
}

class Color {
	construct new(r, g, b, a) {
		_r = r
		_g = g
		_b = b
		_a = a
	}

	construct new(r, g, b) {
	   return Color.new(r, g, b, 1.0)
	}

  construct new(r, g) {
    return Color.new(r, g, 1.0, 1.0)
  }

  construct new(r) {
    return Color.new(r, 1.0, 1.0, 1.0)
  }

  construct new() {
    return Color.new(1.0, 1.0, 1.0, 1.0)
  }

	r=(r) { _r = r }

	r { _r }

	g=(g) { _g = g }

	g { _g }

	b=(b) { _b = b }

	b { _b }

	a=(a) { _a = a }

	a { _a }
}

class Quad {
	construct new(x, y, width, height) {
		_x = x
		_y = y
		_width = width
		_height = height
	}

	x=(x) { _x = x }

	x { _x }

	y=(y) { _y = y }

	y { _y }

	width=(width) { _width = width }
	w=(width) { _width = width }

	width { _width }
	w { _width }

	height=(height) { _height = height }
	h=(height) { _height = height }

	height { _height }
	h { _height }
}

class Transform {
	construct new(ox, oy, r, sx, sy) {
		_ox = ox
		_oy = oy
		_r = r
		_sx = sx
		_sy = sy
	}

  construct new() {
    return Transform.new(0, 0, 0, 1, 1)
  }

	ox=(ox) { _ox = ox }
	ox { _ox }

	oy=(oy) { _oy = oy }
	oy { _oy }

	r=(r) { _r = r }
	r { _r }

	sx=(sx) { _sx = sx }
	sx { _sx }

	sy=(sy) { _sy = sy }
	sy { _sy }
}

foreign class Font {
  construct new_(data, ptsize) {}
	foreign static fromFile(file, ptsize)
	foreign static fromString(str, ptsize)
	foreign static fromEmbedded(ptsize)

  static fromFile(file) {
    return this.fromFile(file, null)
  }

  static fromString(str) {
    return this.fromString(str, null)
  }

  static fromEmbedded() {
    return this.fromEmbedded(null)
  }

	foreign height
	foreign size
  foreign getWidth(text)

	foreign render(text)
}

foreign class Buffer {
  construct new_(mode, data) {}
	foreign static fromFile(file)
  foreign static fromString(str)
  foreign static fromBlank(w, h)

  foreign width
  foreign height

  foreign setAlpha_(alpha)
  foreign setBlend_(mode)
  foreign setColor_(r, g, b, a)
  foreign setClip_(x, y, w, h)

  foreign clone()
  foreign reset()

  foreign clear_(r, g, b, a)
  foreign getPixel_(x, y)
  foreign setPixel_(r, g, b, a, x, y)
  foreign copyPixels_(src, x, y, qx, qy, qw, qh, sx, sy)
  foreign noise(seed, low, high, greyscale)
  foreign floodFill_(x, y, r, g, b, a)
  foreign drawPixel_(x, y, r, g, b, a)
  foreign drawLine_(x0, y0, x1, y1, r, g, b, a)
  foreign drawRect_(x, y, width, height, r, g, b, a)
  foreign drawBox_(x, y, width, height, r, g, b, a)
  foreign drawCircle_(x, y, radius, r, g, b, a)
  foreign drawRing_(x, y, radius, r, g, b, a)
  foreign draw_(src, x, y, qx, qy, qw, qh, ox, oy, r, sx, sy)

  alpha=(alpha) {
    this.setAlpha_(alpha)
  }

  blend=(blend) {
    this.setBlend_(blend)
  }

  color=(color) {
    this.setColor_(color.r, color.g, color.b, color.a)
  }

  clip=(rect) {
    this.setClip_(rect.x, rect.y, rect.w, rect.h)
  }

  clear(color) {
    this.clear_(color.r, color.g, color.b, color.a)
  }

  getPixel(x, y) {
    var c = this.getPixel_(x, y)
    return Color.new(c[0], c[1], c[2], c[4])
  }

  setPixel(x, y, color) {
    this.setPixel_(
      x, y,
      color.r, color.g, color.b, color.a
    )
  }

  copyPixels(src, x, y, quad, sx, sy) {
    this.copyPixels_(
      src, x, y,
      quad.x, quad.y, quad.w, quad.h,
      sx, sy
    )
  }

  copyPixels(src, x, y, sx, sy) {
    this.copyPixels_(
      src, x, y,
      null, null, null, null,
      sx, sy
    )
  }

  floodFill(x, y, color) {
    this.floodFill_(
      x, y,
      color.r, color.g, color.b, color.a
    )
  }

  drawPixel(x, y, color) {
    this.drawPixel_(
      x, y,
      color.r, color.g, color.b, color.a
    )
  }

  drawLine(x0, y0, x1, y1, color) {
    this.drawLine_(
      x0, y0, x1, y1,
      color.r, color.g, color.b, color.a
    )
  }

  drawRect(x, y, width, height, color) {
    this.drawRect_(
      x, y, width, height,
      color.r, color.g, color.b, color.a
    )
  }

  drawBox(x, y, width, height, color) {
    this.drawBox_(
      x, y, width, height,
      color.r, color.g, color.b, color.a
    )
  }

  drawCircle(x, y, radius, color) {
    this.drawCircle_(
      x, y, radius,
      color.r, color.g, color.b, color.a
    )
  }

  drawRing(x, y, radius, color) {
    this.drawRing_(
      x, y, radius,
      color.r, color.g, color.b, color.a
    )
  }

  draw(src, x, y, q, t) {
    this.draw_(
      src, x, y,
      q.x, q.y, q.w, q.h,
      t.ox, t.oy, t.r, t.sx, t.sy
    )
  }

  draw(src, x, y, qt) {
    if (qt is Quad) {
      this.draw_(
        src, x, y,
        qt.x, qt.y, qt.w, qt.h,
        0, 0, 0, 1, 1
      )
    } else if (qt is Transform) {
      this.draw_(
        src, x, y, null, null, null, null,
        qt.ox, qt.oy, qt.r, qt.sx, qt.sy
      )
    }
  }
}

/* foreign class Source {
  construct new_() {}
	foreign static fromData(data)
	foreign static fromBlank()

  foreign length
  foreign state

	foreign callback=(cb)
	foreign destination=(dest)
	foreign gain=(gain)
	foreign pan=(pan)
	foreign rate=(rate)
	foreign loop=(loop)

	foreign play()
	foreign pause()
	foreign stop()
} */

foreign class Data {
  construct new_(data) {}
	foreign static fromFile(file)
	foreign static fromString(str)

  foreign length
	foreign toString
}

foreign class Gif {
	construct new(width, height, ncolors) {}
	foreign update(buf, delay)
	foreign close()
}

class FS {
	foreign static mount(path)
  foreign static unmount(path)
  foreign static setWritePath(path)
  foreign static exists(file)
  foreign static getSize(file)
  foreign static getModified(file)
  foreign static read(file)
  foreign static isDir(file)
  foreign static listDir(path)
  foreign static write(file, data)
  foreign static append(file, data)
  foreign static delete(file)
  foreign static makeDirs(path)
}

class Time {
	foreign static now
	foreign static time

	foreign static sleep(ms)

  static delta { __delta }
  static average { __average }
  static fps { (1 / __average).round }

  static init_() {
    __last = 0
    __delta = 0
    __average = 0
    __avgTimer = 0
    __avgAcc = 1
    __avgCount = 1
  }

  static step() {
    var now = this.now
    if (__last == 0) __last = now
    __delta = now - __last
    __last = now
    __avgTimer = __avgTimer - __delta
    __avgAcc = __avgAcc + __delta
    __avgCount = __avgCount + 1
    if (__avgTimer <= 0) {
      __average = __avgAcc / __avgCount
      __avgTimer = __avgTimer + 1
      __avgCount = 0
      __avgAcc = 0
    }
  }
}

class Graphics {
  foreign static init_(width, height, title, fullscreen, resizable, borderless)

  foreign static fullscreen
  foreign static maxFps

  foreign static setSize(width, height)
  foreign static fullscreen=(fullscreen)
  foreign static maxFps=(fps)

  static init_(conf) {
    __canvas = this.init_(conf.width, conf.height, conf.title, conf.fullscreen, conf.resizable, conf.borderless)
    this.maxFps = conf.maxfps
    __clearColor = Color.new()
    this.clear()
  }

  static width { __canvas.width }

  static height { __canvas.height }

  static clearColor { __clearColor }

  static alpha=(alpha) {
    __canvas.setAlpha(alpha)
  }

  static blend=(blend) {
    __canvas.setBlend(blend)
  }

  static color=(color) {
    __canvas.setColor(color)
  }

  static clip=(rect) {
    __canvas.setClip(rect)
  }

  static clearColor=(color) {
    __clearColor = color
  }

  static clone() {
    return __canvas.clone()
  }

  static reset() {
    __canvas.reset()
  }

  static clear() {
    __canvas.clear(__clearColor)
  }

  static clear(color) {
    __canvas.clear(color)
  }

  static getPixel(x, y) {
    return __canvas.getPixel(x, y)
  }

  static setPixel(x, y, color) {
    __canvas.setPixel(x, y, color)
  }

  static copyPixels(src, x, y, quad, sx, sy) {
    __canvas.copyPixels(src, x, y, quad, sx, sy)
  }

  static copyPixels(src, x, y, sx, sy) {
    __canvas.copyPixels(src, x, y, sx, sy)
  }

  static floodFill(x, y, color) {
    __canvas.floodFill(x, y, color)
  }

  static drawPixel(x, y, color) {
    __canvas.drawPixel(x, y, color)
  }

  static drawLine(x0, y0, x1, y1, color) {
    __canvas.drawLine(x0, y0, x1, y1, color)
  }

  static drawRect(x, y, width, height, color) {
    __canvas.drawRect(x, y, width, height, color)
  }

  static drawBox(x, y, width, height, color) {
    __canvas.drawBox(x, y, width, height, color)
  }

  static drawCircle(x, y, radius, color) {
    __canvas.drawCircle(x, y, radius, color)
  }

  static drawRing(x, y, radius, color) {
    __canvas.drawRing(x, y, radius, color)
  }

  static draw(src, x, y, q, t) {
    __canvas.draw(src, x, y, q, t)
  }

  static draw(src, x, y, qt) {
    __canvas.draw(src, x, y, qt)
  }
}

/* class Audio {
	foreign static init_(rate, buffersize)
} */

class Mouse {
	foreign static visible_=(visible)
	foreign static position_=(position)

  static visible { __visible }

	static position { [__x, __y] }

  static x { __x }

  static y { __y }

	static visible=(visible) {
		this.visible_(visible)
		__visible = visible
	}

	static position=(position) {
		this.position_(position)
		__x = position[0]
		__y = position[1]
	}

	static x=(x) {
		this.position = [x, this.y]
	}

	static y=(y) {
		this.position = [this.x, y]
	}

  static reset() {

  }

  static onEvent_(e) {

  }
}

class KeyBoard {
  static init_() {
    __keysDown = {}
    __keysPressed = {}
  }

  static isDown(keys) {
    if (keys is List) {
      for (k in keys) {
        if (!__keysDown[k]) {
          return false
        }
      }
    }
    return __keysDown[keys]
  }

  static wasPressed(keys) {
     if (keys is List) {
      for (k in keys) {
        if (!__keysPressed[k]) {
          return false
        }
      }
    }
    return __keysPressed[keys]
  }

  static reset() {
    for (k in __keysPressed.keys) {
      __keysPressed[k] = false
    }
  }

  static onEvent_(e) {
    if (e[0] == "keydown") {
      __keysDown[e[1]] = true
      __keysPressed[e[1]] = true
      System.print(e[1])
    } else if (e[0] == "keyup") {
      __keysDown[e[1]] = false
      System.print(e[1])
    }
  }
}

/* class BufferFX {
	foreign static desaturate(buf, amount)
	foreign static palette(buf, pal)
	foreign static dissolve(buf, amount, seed)
	foreign static mask(buf, mask, channel)
	foreign static wave(buf, src, ax, ay, sx, sy, ox, oy)
	foreign static displace(buf, src, map, cx, cy, sx, sy)
	foreign static blur(buf, src, rx, ry)
} */



class Cactus {
	foreign static version

  foreign static poll()
  foreign static exit(code)
  foreign static info(info)

  static exit() {
    this.exit(Exit.SUCCESS)
  }

  static onInit_() {
    Graphics.init_(Config.new())
    Time.init_()
    KeyBoard.init_()
  }

  static onQuit_() {
    this.exit(Exit.SUCCESS)
  }

  static onStepMain {
    return Fn.new {
      for (e in this.poll()) {
        if (e[0] == "quit")                   {
          this.onQuit_()
        } else if (e[0] == "resize")          {
          Graphics.setSize(e[1], e[2])
        }
        
        KeyBoard.onEvent_(e)
        Mouse.onEvent_(e)
      }

      Time.step()
      /* UPDATE METHOD */
      Graphics.clear()
      /* DRAW METHOD */
      /* DEBUG METHOD */
      KeyBoard.reset()
      Mouse.reset()
    }
  }

  static onStep_() {
    Fiber.new(this.onStepMain).try()
    System.gc()
    System.gc()
  }
}