local _ = require "color"
Color = _[1]

local util = {}

local stroke = {
  { -1, -1 }, { -1,  0 }, { -1,  1 },
  { -0, -1 },             { -0,  1 },
  {  1, -1 }, {  1,  0 }, {  1,  1 },
}

local nstroke = {
  { -1,  1, 1 },
  {  1,  1, 1 },
  {  0,  2, 1 },
  { -1,  0, 0 },
  {  1,  0, 0 },
  {  0, -1, 0 },
  {  0,  1, 0 },
}

function util.print(txt, x, y, c1, c2)
  G.framebuffer:reset()
  local s = 2
  local txt = G.font:render(txt)
  for i = 1, #nstroke do
    local x = (x - s) + (1 + nstroke[i][1]) * s
    local y = (y - s) + (1 + nstroke[i][2]) * s
    if nstroke[i][3] == 1 then
      G.framebuffer:setColor(unpack(Color[c2]))
    else
      G.framebuffer:setColor(unpack(Color[c1]))
    end
    G.framebuffer:draw(txt, x, y)
  end
  G.framebuffer:setColor(unpack(Color["black"]))
  G.framebuffer:draw(txt, x, y)

  G.framebuffer:reset()
end

function util.draw_stroke(str, x, y, c)
  G.framebuffer:reset()
  local txt = G.font:render(str)
  G.framebuffer:setColor(unpack(Color[c]))
  for i = 1, #stroke do
    G.framebuffer:draw(txt, x + stroke[i][1], y + stroke[i][2] - 2)
  end
  G.framebuffer:setColor(unpack(Color["black"]))
  G.framebuffer:draw(txt, x, y - 2)
  G.framebuffer:reset()
end

return util
