--[[
  Let's assume you want to draw an N-sided polygon of radius r, centred at (0,0). Then the n vertices are given by:

  x[n] = r * cos(2*pi*n/N)
  y[n] = r * sin(2*pi*n/N)
  where 0 <= n < N. Note that cos and sin here are working in radians, not degrees (this is pretty common in most programming languages).

  If you want a different centre, then just add the coordinates of the centre point to each (x[n], y[n]). If you want a different orientation, you just need to add a constant angle. So the general form is:

  x[n] = r * cos(2*pi*n/N + theta) + x_centre
  y[n] = r * sin(2*pi*n/N + theta) + y_centre
]]

local sin, cos, pi = math.sin, math.cos, math.pi
local min, max, sqrt = math.min, math.max, math.sqrt
local rot = 0
local str_sides = ""
local num_sides = 1
local font = sol.Font.fromEmbedded(8)

local function drawShapes()
  while true do
    local x = G.screen:getWidth() / 2
    local y = G.screen:getHeight() / 2
    for sides = 3, ((num_sides > 3) and num_sides or 3) do
      local radius = ((sides^2) * (sin(rot) * sqrt(2)) * sides) / sides
      local rotation = ((sides % 2 == 0) and rot or -rot)
      local r, g, b = min(1, (sides^2) / 255), min(1, (sides^2) / 255), min(1, (sides^2) / 255)
      G.screen:drawPolygon(x, y, sides, radius, rotation, r, g, b)
    end
    coroutine.yield()
  end
end

local drawShapes = coroutine.wrap(drawShapes)


function sol.onLoad(dt)
  math.randomseed(sol.time.getNow())
  sol.debug.setVisible(true)
  G.screen = sol.Buffer.fromBlank(G.width, G.height)

end


function sol.onUpdate(dt)
  if sol.keyboard.wasPressed("escape") then sol.system.quit() end
  rot = rot + dt
end


function sol.onKeyDown(key, char)
  if key == "return" and #str_sides > 0 then
    num_sides = tonumber(str_sides)
    str_sides = ""
  elseif key == "backspace" then
    str_sides = str_sides:sub(1, -2)
  elseif char and char:match("%d+") then
    str_sides = str_sides .. char
  end
end


function sol.onDraw()
  G.screen:clear(0,0,0,1)
  -- G.screen:setBlend("add")
  drawShapes()
  G.screen:copyPixels(font:render(str_sides), 2, G.width - (font:getHeight() - 2))
  sol.graphics.draw(G.screen, 0, 0, nil, nil, G.scale)
end
