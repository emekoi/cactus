local util = require "util"
local _ = require "color"
Color0 = _[1]
Color1 = _[2]

local demo = {
  menu     = {},
  hello    = { next =  "vortex" },
  vortex   = { next = "sphere" },
  sphere   = { next = "drippy" },

  drippy   = { next = "rad_bot" },
  rad_bot  = { next = "ripple" },
  ripple   = { next = "smoke" },

  smoke    = { next = "wave" },
  wave     = { next = "wheel" },
  wheel    = { next = "carpet" },

  carpet   = { next = "torus" },
  torus    = { next = "whorl" },
  whorl    = { next = "hello" },
}

local sz = 0
for _, v in pairs(demo) do
  -- v.__index = v
  sz = sz + 1
end
demo.len = sz

function demo.menu:init()
  -- demo
  self.tick = 0
  self.cursor = { demo = "demo", pos = 1}
end

function demo.menu:update(dt)
  util.draw_stroke(self.cursor.pos, 0, 0, "pink")
  if sol.keyboard.wasPressed(unpack(G.keys.up)) then
    self.cursor.pos = math.max(self.cursor.pos - 1, 1)
  elseif sol.keyboard.wasPressed(unpack(G.keys.down)) then
    self.cursor.pos = math.min(self.cursor.pos + 1, demo.len)
  end
  if sol.keyboard.wasPressed(unpack(G.keys.action)) then
    for k, v in pairs(demo) do
  		if type(v) == "table" then
        if self.cursor.pos == v.n then
          v:init()
          demo.update = v.update
          demo.draw = v.draw
        end
      end
    end
  end
  self.tick = self.tick + dt / 5
end

function demo.menu:draw()
  local i = 1
  for k, v in pairs(demo) do
		if type(v) == "table" and k ~= "menu" then
      v.n = i
      -- print(k, v.n)
			local str = k
			for c = 1, #k do
	  		if k:sub(c, c) == "_" then
	   			str = k:sub(1, c - 1) .. " " .. k:sub(c + 1, #k)
	  		end
	 		end
			local x = self.cursor.pos == i and 24 + G.font:getWidth("_") or 24
			local y = 8 + (G.font:getHeight(str) * i )
			util.draw_stroke(str, x, 8 + (G.font:getHeight(str) * i ), "pink")
			if self.tick % 0.2 < 0.15 and self.cursor.pos == v.n then
				util.draw_stroke(">", x - G.font:getWidth("_"), 8 + (G.font:getHeight(str) * i ), "pink")
			end
			i = i + 1
		end
  end
end

function demo.back()
  if sol.keyboard.wasPressed(unpack(G.keys.back)) then
    demo["menu"]:init()
    demo.update = demo["menu"].update
    demo.draw = demo["menu"].draw
  end
end

function demo.hello:init()
  self = demo.hello
  self.tick = 0
  self.str = "Hello World"
end

function demo.hello:update(dt)
  self = demo.hello
  for i = 1, #self.str do
    for j0 = 0, 7 do
    -- j0 = 0
      local j = 7 - j0
      local c = 7 + j  - 1
      -- local c = 1
      local t1 = self.tick + i * 4 - j * 2
      local x = math.cos(0) * 10
      local y = 38 + j0 + math.cos(t1/50) * 5
      G.framebuffer:reset()
      G.framebuffer:setColor(unpack(Color1[c]))
      local txt = G.font:render(self.str:sub(i,i))
      G.framebuffer:draw(txt, (x+10)*i, y)
    end
  end
  self.tick = self.tick + 1
end

function demo.hello:draw()

end

demo.init = demo["menu"].init
demo.update = demo["menu"].update
demo.draw = demo["menu"].draw


return demo
