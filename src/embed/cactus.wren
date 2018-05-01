/**
 * Copyright (c) 2018 emekoi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

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
		_resizable   = false
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
	construct pixel(r, g, b, a) {
		_r = r
		_g = g
		_b = b
		_a = a
	}

	construct color(r, g, b) {
		this.pixel(r, g, b, 1.0)
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

class Rect {
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

// wren_open_font,
foreign class Font {
	foreign static fromFile(file, ptsize)
	foreign static fromString(str, ptsize)
	foreign static fromEmbedded(ptsize)

  foreign width
	foreign height
	foreign size

	foreign render(text)
}

// wren_open_buffer,
foreign class Buffer {
  foreign new_(mode, data)
	foreign static fromFile(file)
  foreign static fromString(str)
  foreign static fromBlank(width, height)

  foreign width
  foreign height

  foreign alpha=(alpha)
  foreign blend=(mode)
  foreign color=(color)
  foreign clip=(rect)

  foreign clone()
  foreign reset()
  foreign clear(color)
  foreign getPixel(x, y)
  foreign setPixel(color, x, y)
  foreign copyPixels(src, x, y, sub, sx, sy)
  foreign noise(seed, low, high, greyscale)
  foreign floodFill(color, x, y)
  foreign drawPixel(color, x, y)
  foreign drawLine(color, x0, y0, x1, y1)
  foreign drawRect(color, x, y, width, height)
  foreign drawBox(color, x, y, width, height)
  foreign drawCircle(color, x, y, radius)
  foreign drawRing(color, x, y, radius)
  foreign draw(src, x, y, sub, transform)
}

// wren_open_source,
foreign class Source {
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
}

// wren_open_data,
foreign class Data {
  construct new_(data) {}
	foreign static fromFile(file)
	foreign static fromString(str)

  foreign length

	foreign toString()
}

// wren_open_gif,
foreign class Gif {
	construct new(width, height, ncolors) {}
	foreign update(buf, delay)
	foreign close()
}

// wren_open_cactus, + wren_open_system,
class Cactus {
  foreign static init_(config)

	foreign static version

  foreign static poll()
  foreign static info(info)
}

// wren_open_fs,
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

// wren_open_time
class Time {
	foreign static now
	foreign static time

	foreign static sleep(ms)
}

// wren_open_graphics,
class Graphics {
  foreign static init_(width, height, title, fullscreen, resizable, borderless)

  foreign static fullscreen
  foreign static maxFps

  foreign static size=(size)
  foreign static fullscreen=(fullscreen)
  foreign static maxFps=(fps)
}

// wren_open_audio,
class Audio {
	foreign static init_(rate, buffersize)
}

// wren_open_mouse
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
}


// wren_open_bufferfx,
class BufferFX {
	foreign static desaturate(buf, amount)
	foreign static palette(buf, pal)
	foreign static dissolve(buf, amount, seed)
	foreign static mask(buf, mask, channel)
	foreign static wave(buf, src, ax, ay, sx, sy, ox, oy)
	foreign static displace(buf, src, map, cx, cy, sx, sy)
	foreign static blur(buf, src, rx, ry)
}
