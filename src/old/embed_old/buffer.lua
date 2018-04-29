--
-- Copyright (c) 2015 rxi
--
-- This library is free software; you can redistribute it and/or modify it
-- under the terms of the MIT license. See LICENSE for details.
--


local function checkArg(idx, cond, msg)
  if not cond then
    error("bad argument #" .. idx .. ", " .. msg, 3)
  end
end


function sol.Buffer:getSize()
  return self:getWidth(), self:getHeight()
end


local defaultFont = sol.Font.fromEmbedded()

local fontTexCache = {}
setmetatable(fontTexCache, {
  __index = function(t, k)
    fontTexCache[k] = {}
    return fontTexCache[k]
  end,
  __mode = "v",
})

function sol.Buffer:drawText(font, text, x, y, width)
  if type(font) ~= "userdata" then
    return self:drawText(defaultFont, font, text, x, y, width)
  end
  checkArg(3, x == nil or type(x) == "number", "expected number")
  checkArg(4, y == nil or type(y) == "number", "expected number")
  checkArg(5, width == nil or type(width) == "number", "expected number")
  text = tostring(text)
  if width then
    -- Word wrapped multi line
    local height = font:getHeight()
    local line
    for word in text:gmatch("%S+") do
      local tmp = (line and (line .. " ") or "") .. word
      if font:getWidth(tmp) > width then
        sol.graphics.drawText(font, line, x, y)
        y = y + height
        line = word
      else
        line = tmp
      end
    end
    self:drawText(font, line, x, y)
  elseif text:find("\n") then
    -- Multi line
    local height = font:getHeight()
    for line in (text.."\n"):gmatch("(.-)\n") do
      self:drawText(font, line, x, y)
      y = y + height
    end
  else
    -- Single line
    local tex = fontTexCache[font][text]
    if not tex then
      tex = font:render(text)
      fontTexCache[font][text] = tex
    end
    self:drawBuffer(tex, x, y)
  end
end


local sin, cos, pi, max = math.sin, math.cos, math.pi, math.max

function sol.Buffer:drawPolygon(x, y, sides, radius, rot, r, g, b, a)
  checkArg(1, x == nil or type(x) == "number", "expected number")
  checkArg(2, y == nil or type(y) == "number", "expected number")
  checkArg(3, sides == nil or type(sides) == "number", "expected number")
  checkArg(4, type(radius or 0) == "number", "expected number")

  local x_coords, y_coords = {}, {}

  -- Find the vertices of the polygon
  for n = 1, sides do
    x_coords[n] = radius * cos(2 * pi * n / sides + (rot and rot or 0)) + x
    y_coords[n] = radius * sin(2 * pi * n / sides + (rot and rot or 0)) + y
  end

  x_coords[sides + 1] = x_coords[1]
  y_coords[sides + 1] = y_coords[1]

  -- Draw the lines between each vertice
  for n = 1, sides + 1 do
    local x, y = x_coords[n], y_coords[n]
    local x1, y1 = x_coords[max(1, n - 1)], y_coords[max(1, n -1 )]
    self:drawLine(x, y, x1, y1, r, g, b, a)
  end
end
