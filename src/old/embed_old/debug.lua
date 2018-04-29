--
-- Copyright (c) 2015 rxi
--
-- This library is free software; you can redistribute it and/or modify it
-- under the terms of the MIT license. See LICENSE for details.
--


sol.debug = sol.debug or {}

local font
local inited = false
local enabled = false
local focused = false
local indicators = {}
local lines = {}
local history = {}
local vursor = 1
local inputbuf = ""
local enputbuf = ""
local outputbuf = ""
local size, cursor = 0, 0

-- Override print
local _print = print
print = function(...)
  local info = debug.getinfo(2, "Sl")
  io.write(info.short_src .. ":" .. info.currentline .. " ")
  io.flush()
  _print(...)
  -- Convert all arguments to string and store in table
  local t = {}
  for i = 1, select("#", ...) do
    t[#t + 1] = tostring(select(i, ...))
  end
  local str = table.concat(t, " ")
  -- Multiple lines? Split and insert, else just insert the string
  if str:match("\n") then
    for line in (str .. "\n"):gmatch("(.-)\n") do
      table.insert(lines, line)
    end
  else
    table.insert(lines, str)
  end
  while #lines > 6 do
    table.remove(lines, 1)
  end
end


local indicatorIdx
local textRegionWidth = 20

local function newIndicator(fn, min, max)
  min = min or 0
  max = max or 0
  local trueMin, trueMax = min, max
  -- Get idx
  indicatorIdx = indicatorIdx and (indicatorIdx + 1) or 0
  -- Init
  local pad = 8
  local height = 26
  local maxBars = 16
  local barUpdatePeriod = 1
  local yoffset = pad + height * indicatorIdx
  local lastUpdate = sol.time.getNow()
  local bars = {}
  local lastMin = min
  local lastMax = max
  -- Fill bars with zeros
  for i = 1, maxBars do
    bars[i] = 0
  end
  -- Return draw function
  return function()
    local txt, val = fn()
    -- Resize text region?
    textRegionWidth = math.max(font:getWidth(txt) + 8, textRegionWidth)
    -- Update bars?
    if sol.time.getNow() > lastUpdate + barUpdatePeriod then
      table.remove(bars)
      table.insert(bars, 1, val)
      min = math.min(trueMin, unpack(bars))
      max = math.max(trueMax, unpack(bars))
      lastUpdate = sol.time.getNow()
    end
    -- Draw text
    local w = textRegionWidth
    sol.graphics.drawRect(pad / 2, yoffset - (pad / 2),
                           w, height - 1, 0, 0, 0, .8)
    sol.graphics.drawText(font, txt, pad, yoffset)
    -- Draw bars
    sol.graphics.drawRect(pad / 2 + w + 1, yoffset - (pad / 2),
                           73, height - 1, 0, 0, 0, .8)
    for i, v in ipairs(bars) do
      local x = math.floor((bars[i] - min) / (max - min) * 16)
      sol.graphics.drawRect(pad / 2 + w + 1 + (i - 1) * 4 + 5,
                             yoffset + 16 - x, 3, x,
                             nil, nil, nil, (i == 1) and 1 or .4)
    end
  end
end

local function slice(str, pos)
  return str:sub(1, pos), str:sub(pos + 1)
end


local function draw()
  -- Not enabled? Don't draw
  if not enabled then
    return
  end
  -- Draw
  sol.graphics.reset()
  -- Draw indicators
  for i, v in ipairs(indicators) do
    v()
  end
  -- Draw console input text
  local w = 300
  if focused then
    local h = font:getHeight()
    local y = sol.graphics.getHeight() - 8 - h
    local caret = (sol.time.getTime() % .6 < .3) and "_" or " "
    w = math.max(w, font:getWidth(inputbuf .. "_" .. enputbuf))
    sol.graphics.drawRect(4, sol.graphics.getHeight() - h - 12,
                           w + 8, h + 8,
                           0, 0, 0, .8)
    sol.graphics.drawText(font, "> " .. inputbuf .. caret .. enputbuf, 8, y)
  end
  -- Draw console output text
  if #lines > 0 then
    local h = font:getHeight()
    local rh = #lines * h + 8
    local oy = focused and (h + 9) or 0
    for i, v in ipairs(lines) do
      w = math.max(w, font:getWidth(v))
    end
    sol.graphics.drawRect(4, sol.graphics.getHeight() - 4 - rh - oy,
                           w + 8, rh,
                           0, 0, 0, .8)
    for i, v in ipairs(lines) do
      local y = sol.graphics.getHeight() - 8 - (#lines - i + 1) * h
      sol.graphics.drawText(font, v, 8, y - oy)
    end
  end
end


local function init()
  -- Init font
  font = sol.Font.fromEmbedded(12)
  -- Init indicators
  sol.debug.addIndicator(function()
    local r = sol.time.getFps()
    return r .. "fps", r
  end)
  sol.debug.addIndicator(function()
    local r = collectgarbage("count")
    return string.format("%.2fmb", r / 1024), r
  end)
  -- Override present function to draw the debug information before calling the
  -- proper present function
  local present = sol.graphics.present
  sol.graphics.present = function(...)
    draw()
    present(...)
  end
  -- Set init flag
  inited = true
end


local onError = function(msg)
  print("error: " .. msg:match("[^\n]+"))
end

function sol.debug._onEvent(e)
  -- Handle console's keyboard input
  if enabled and focused then
    if e.type == "keydown" then
      if e.key == "backspace" then
        local _, temp = slice(outputbuf, cursor)
        size = math.max(0, #outputbuf - 1)
        cursor = math.max(0, cursor - 1)
        outputbuf = _:sub(1, #_ - 1) .. temp
        size = #outputbuf
        history[1] = outputbuf
      elseif e.key == "tab" then
        local _, temp = slice(outputbuf, cursor)
        cursor = cursor + 2
        outputbuf = _ .. "  " .. temp
        size = #outputbuf
        history[1] = outputbuf
      elseif e.key == "home" then
        cursor = 0
      elseif e.key == "end" then
        cursor = size
      elseif e.key == "right" then
        cursor = math.min(#outputbuf, cursor + 1)
      elseif e.key == "left" then
        cursor = math.max(0, cursor - 1)
      elseif e.key == "up" then
        vursor = math.min(#history,vursor + 1)
        outputbuf = history[vursor]
        size = #outputbuf
        cursor = size
      elseif e.key == "down" then
        vursor = math.max(1,vursor - 1)
        outputbuf = history[vursor]
        size = #outputbuf
        cursor = size
      elseif e.key == "return" then
        local fn, err = loadstring(outputbuf, "=input")
        if fn then
          xpcall(fn, onError)
        else
          onError(err)
        end
        table.insert(history, 2, outputbuf)
        outputbuf = ""
        inputbuf, enputbuf = "", ""
        size, cursor = 0, 0
        history[1] = ""
      end
    elseif e.type == "textinput" then
      local _, temp = slice(outputbuf, cursor)
      cursor = cursor + #e.text
      outputbuf = _ .. e.text .. temp
      size = #outputbuf
      history[1] = outputbuf
    end
    inputbuf, enputbuf = slice(outputbuf, cursor)
  end
end


function sol.debug._draw()
  draw()
end


function sol.debug.setVisible(x)
  enabled = x and true or false
  if enabled and not inited then
    init()
  end
end

function sol.debug.getVisible(x)
  return enabled
end


function sol.debug.setFocused(x)
  focused = x and true or false
end

function sol.debug.getFocused(x)
  return focused
end


function sol.debug.clear()
  while #lines > 0 do
    table.remove(lines)
  end
end


function sol.debug.addIndicator(fn, min, max)
  -- Error check
  local str, num = fn()
  if type(str) ~= "string" or type(num) ~= "number" then
    error("expected indicator function to return string and number", 2)
  end
  if min and type(min) ~= "number" then
    error("expected `min` to be a number", 2)
  end
  if max and type(max) ~= "number" then
    error("expected `max` to be a number", 2)
  end
  -- Create, add and return
  local indicator = newIndicator(fn, min, max)
  table.insert(indicators, indicator)
  return indicator
end


function sol.debug.removeIndicator(indicator)
  for i, v in ipairs(indicators) do
    if v == indicator then
      table.remove(indicators, i)
      return
    end
  end
end
