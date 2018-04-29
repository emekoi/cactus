local _ = require "color"
Color = _[1]
local util = require "util"
local demo = require "demos"

function sol.onLoad()
	math.randomseed(sol.time.getNow())
  G.framebuffer = sol.Buffer.fromBlank(G.width, G.height)
  G.postbuffer = G.framebuffer:clone()
  G.font = sol.Font.fromEmbedded(16)
  G.bgcolor = Color["peach"]
  G.state = "menu"
  G.keys = {
    up     = { "up", "w" },
    down   = { "down", "s" },
    action = { "z", "space", "enter" },
    back   = { "x" },
  }
	demo:init()
end

function sol.onUpdate(dt)
  require("stalker").update()
  -- util.print(G.font, "tick: " .. G.tick, 0, 0, "indigo", "dark-purple")
	demo:update(dt)
	demo.back()
  collectgarbage()
  collectgarbage()
end

function sol.onDraw()
	demo:draw()
  G.postbuffer = G.framebuffer:clone()
  G.framebuffer:clear(unpack(G.bgcolor))
  G.framebuffer:reset()
  sol.graphics.copyPixels(G.postbuffer, 0, 0, nil, G.scale, G.scale)
end

function sol.onKeyDown(k)
	if k == "escape" then
		sol.system.quit()
	elseif k == "r" then
    sol.onLoad()
  end
end
