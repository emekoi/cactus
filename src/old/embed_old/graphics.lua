--
-- Copyright (c) 2015 rxi
--
-- This library is free software; you can redistribute it and/or modify it
-- under the terms of the MIT license. See LICENSE for details.
--

-- Override sol.graphics.init function
local init = sol.graphics.init

sol.graphics.init = function(...)
  -- Do init
  sol.graphics.screen = init(...)
  screen = sol.graphics.screen
  -- Bind the screen buffer's methods to the graphics module
  for k, v in pairs(sol.Buffer) do
    if not sol.graphics[k] then
      sol.graphics[k] = function(...)
        return v(screen, ...)
      end
    end
  end
  -- Unbind Buffer constructors (which make no sense bound)
  sol.graphics.fromBlank  = nil
  sol.graphics.fromFile   = nil
  sol.graphics.fromString = nil
  -- Override sol.graphics.clear() to use _clearColor if available
  local clear = sol.graphics.clear
  function sol.graphics.clear(r, g, b, a)
    local c = sol.graphics._clearColor
    r = r or (c and c[1])
    g = g or (c and c[2])
    b = b or (c and c[3])
    clear(r, g, b, 1)
  end
  -- Return main screen buffer
  return screen
end


function sol.graphics.setClearColor(...)
  sol.graphics._clearColor = { ... }
end


function sol.graphics.getClearColor(...)
  return unpack(sol.graphics._clearColor)
end


local currentBuffer = sol.graphics.screen

function sol.graphics.setBuffer(buf)
  local buf = buf or sol.graphics.screen
  if buf ~= currentBuffer then
    for k, v in pairs(sol.graphics) do
      if type(v) == "function" then
        sol.graphics[k] = function(...)
          local gfunc = v
          if not buf[k] or k == "clear" then
            return gfunc(...)
          else
            return buf[k](buf, ...)
          end
        end
      end
    end
    currentBuffer = buf
  end
end


function sol.graphics.withBuffer(func, buf)
  sol.graphics.setBuffer(buf)
  func()
  sol.graphics.setBuffer()
end


function sol.graphics._onEvent(e)
  if e.type == "resize" then
    local w, h = sol.graphics.getSize()
    sol.graphics.setSize(e.width, e.height)
  end
end
