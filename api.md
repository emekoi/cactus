### Callbacks

##### `sol.onLoad()`
##### `sol.onUpdate(dt)`
##### `sol.onDraw()`
##### `sol.onKeyDown(key, char)`
##### `sol.onKeyUp(key)`
##### `sol.onMouseMove(x,y)`
##### `sol.onMouseDown(x, y, button)`
##### `sol.onMouseUp(x, y, button)`
##### `sol.onQuit()`
##### `sol.onError(msg, stacktrace)`
##### `sol.getVersion()`
##### `sol._pcall(fn [, ...])`
##### `sol._argv`

### sol.Data
##### `sol.Data.fromFile(filename)`
##### `sol.Data.fromString(string)`
##### `Data:getLength()`
##### `Data:toString()`

### sol.Source
##### `sol.Source.fromBlank()`
##### `sol.Source.fromData(data)`
##### `Source:setCallback(callback)`
##### `Source:setDestination([dest])`
##### `Source:setGain([gain])`
##### `Source:setPan([pan])`
##### `Source:setLoop([loop])`
##### `Source:setRate([rate])`
##### `Source:getState()`
##### `Source:getLength()`
##### `Source:play([reset])`
##### `Source:pause()`
##### `Source:stop()`

### sol.Buffer
##### `sol.Buffer.fromFile(filename)`
##### `sol.Buffer.fromString(string)`
##### `sol.Buffer.fromBlank(w, h)`
##### `Buffer:setPixel(x, y, r, g, b, a)`
##### `Buffer:setAlpha(alpha)`
##### `Buffer:setClip(x, y, w, h)`
##### `Buffer:setBlend(mode)`
##### `Buffer:setColor(r, g, b, a)`
##### `Buffer:getWidth()`
##### `Buffer:getHeight()`
##### `Buffer:getSize()`
##### `Buffer:getPixel(x, y)`
##### `Buffer:floodFill(x, y [, r, g, b, a])`
##### `Buffer:drawPixel(x, y [, r, g, b, a])`
##### `Buffer:drawLine(x1, y1, x2, y2 [, r, g, b, a])`
##### `Buffer:drawRect(x, y, w, h [, r, g, b, a])`
##### `Buffer:drawBox(x, y, w, h [, r, g, b, a])`
##### `Buffer:drawCircle(x, y, radius [, r, b, g, a])`
##### `Buffer:drawRing(x, y, radius [, r, b, g, a])`
##### `Buffer:drawPolygon(x, y, side, radius, rot, r, g, b, a)`
##### `Buffer:drawText([font,] text, x, y [, width])`
##### `Buffer:drawBuffer(src, x, y [, sub, rot, sx, sy, ox, oy])`
##### `Buffer:draw(src, x, y [, sub, rot, sx, sy, ox, oy])`
##### `Buffer:copyPixels(src, x, y [, sub, sx, sy])`
##### `Buffer:noise([seed, low, high, grey])`
##### `Buffer:clone()`
##### `Buffer:reset()`

### sol.Gif
##### `sol.Gif.new(w, h, ncolors)`
##### `Gif:update(buf, delay)`
##### `Gif:close()`

### sol.Font
##### `sol.Font.fromFile(filename, size)`
##### `sol.Font.fromString(string, size)`
##### `sol.Font.fromEmbedded(size)`
##### `Font:render(string)`
##### `Font:getWidth(string)`
##### `Font:getHeight()`

### sol.mouse
##### `sol.mouse.isDown(key)`
##### `sol.mouse.wasPressed(key)`
##### `sol.mouse.setVisible(visible)`
##### `sol.mouse.getPosition()`
##### `sol.mouse.setPosition(x, y)`
##### `sol.mouse.getX()`
##### `sol.mouse.getY()`

### sol.system
##### `sol.system.info(type)`
##### `sol.system.poll()`
##### `sol.system.quit()`

### sol.audio
##### `sol.audio.master`

### sol.fs
##### `sol.fs.append(filename, data)`
##### `sol.fs.getSize(filename)`
##### `sol.fs.delete(filename)`
##### `sol.fs.isDir(filename)`
##### `sol.fs.unmount(path)`
##### `sol.fs.getModified(filename)`
##### `sol.fs.exists(filename)`
##### `sol.fs.mount(path)`
##### `sol.fs.setWritePath(path)`
##### `sol.fs.write(filename, data)`
##### `sol.fs.read(filename)`
##### `sol.fs.listDir(path)`
##### `sol.fs.makeDirs(path)`

### sol.time
##### `sol.time.getAverage()`
##### `sol.time.getNow()`
##### `sol.time.getTime()`
##### `sol.time.getDelta()`
##### `sol.time.sleep(seconds)`
##### `sol.time.step()`
##### `sol.time.getFps()`

### sol.graphics
##### `sol.graphics.setColor(r,g,b,a)`
##### `sol.graphics.drawCircle(x, y, radius, r, b, g, a)`
##### `sol.graphics.getPixel(x, y)`
##### `sol.graphics.drawBuffer(src, x, y, sub, rot, sx, sy, ox, oy)`
##### `sol.graphics.floodFill(x, y, r, g, b, a)`
##### `sol.graphics.copyPixels(src, x, y, sub, sx, sy)`
##### `sol.graphics.setFullscreen(fullscreen)`
##### `sol.graphics.getFullscreen()`
##### `sol.graphics.setBlend(mode)`
##### `sol.graphics.drawRing(x, y, radius, r, b, g, a)`
##### `sol.graphics.drawPolygon(x, y, side, radius, rot, r, g, b, a)`
##### `sol.graphics.getWidth()`
##### `sol.graphics.setClearColor(r, g, b, a)`
##### `sol.graphics.getClearColor()`
##### `sol.graphics.getHeight()`
##### `sol.graphics.clear(r,g,b,a)`
##### `sol.graphics.reset()`
##### `sol.graphics.setSize(width, height)`
##### `sol.graphics.drawBox(x, y, w, h, r, g, b, a)`
##### `sol.graphics.clone()`
##### `sol.graphics.setAlpha(alpha)`
##### `sol.graphics.setPixel(x, y, r, g, b, a)`
##### `sol.graphics.noise(seed, low, high, grey)`
##### `sol.graphics.getSize()`
##### `sol.graphics.drawPixel(x, y, r, g, b, a)`
##### `sol.graphics.drawPixel(x, y, r, g, b, a)`
##### `sol.graphics.draw(src, x, y, sub, rot, sx, sy, ox, oy)`
##### `sol.graphics.drawRect(x, y, w, h, r, g, b, a)`
##### `sol.graphics.setClip(x, y, w, h)`
##### `sol.graphics.drawLine(x1, y1, x2, y2, r, g, b, a)`
##### `sol.graphics.drawText(font, text, x, y, width)`

### sol.bufferfx
##### `sol.bufferfx.desaturate(self, amount)`
##### `sol.bufferfx.dissolve(self,amount, s? investigate later)`
##### `sol.bufferfx.blur(self, src, rx, ry, )`
##### `sol.bufferfx.wave(self, src, ax, ay, sx, sy, ox, oy)`
##### `sol.bufferfx.mask(self, mask, channel)`
##### `sol.bufferfx.palette(self, palette)`
##### `sol.bufferfx.displace(self, src, map, cx, cy, sx, sy)`

### sol.debug
##### `sol.debug.setVisible(visible)`
##### `sol.debug.setFocused(focused)`
##### `sol.debug.getVisible()`
##### `sol.debug.getFocused()`
##### `sol.debug.removeIndicator(indicator)`
##### `sol.debug.addIndicator(fn, min, max)`
##### `sol.debug.clear()`

### sol.keyboard
##### `sol.keyboard.isDown(key, ...)`
##### `sol.keyboard.wasPressed(key, ...)`

### Config
##### `config.title`
##### `config.width`
##### `config.height`
##### `config.maxfps`
##### `config.samplerate`
##### `config.buffersize`
##### `config.fullscreen`
##### `config.resizable`
##### `config.borderless`
##### `config.identity`
