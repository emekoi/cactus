local width, height = sol.graphics.getSize()
local txt = sol.Font.fromEmbedded(32):render("NO PROJECT LOADED")
local txtPost = txt:clone()
local txtMask = txt:clone()
local particle_count = 80
local particles = {}

local stroke = {
  { -1, -1 }, { -1,  0 }, { -1,  1 },
  { -0, -1 },             { -0,  1 },
  {  1, -1 }, {  1,  0 }, {  1,  1 },
}

local function round(x)
  return x >= 0 and math.floor(x + .5) or math.ceil(x - .5)
end

local function random(a, b)
  if not a then a, b = 0, 1 end
  if not b then b = 0 end
  return a + math.random() * (b - a)
end

function sol.onLoad()
	math.randomseed(sol.time.getNow())
  sol.debug.setVisible(true)
	sol.graphics.setClearColor(0.15, 0.15, 0.15)
  for i = particle_count, 1, -1 do
    local p = {
  		x  = random(width),
      y  = random(height),
  		vx = random(1.25) * ({-1, 1})[round(random(1, 2))],
  		vy = random(1.25) * ({1, -1})[round(random(1, 2))],
		}
  	particles[i] = p
  end
end

function sol.onUpdate(dt)
  for i = particle_count, 1, -1 do
		local p = particles[i]
		if p.x >= (width + 16) or p.x < -16 then
			p.vx = -p.vx
		end
		if p.y >= (height + 16) or p.y < -16 then
			p.vy = -p.vy
		end
		p.x = p.x + p.vx
  	p.y = p.y + p.vy
  end
  collectgarbage()
  collectgarbage()
end

function sol.onDraw()
  -- Draw particles
	sol.graphics.setBlend("add")
  for i = particle_count, 1, -1 do
  	local p = particles[i]
    sol.graphics.drawCircle(p.x, p.y, 4)
	  for j = particle_count, 1, -1 do
		  local o = particles[j]
      local dist = math.sqrt((p.x - o.x)^2 + (p.y - o.y)^2)
			if math.sqrt((p.x - o.x)^2 + (p.y - o.y)^2) < 64 then
        local r, g, b = 1, 1, 1
        a = ((64 - math.sqrt((p.x - o.x)^2 + (p.y - o.y)^2)) * 4) / 255
				sol.graphics.drawLine(p.x, p.y, o.x, o.y, r, g, b, a)
			end
		end
  end
	-- Draw text
	local n = sol.time.getTime() * 2
  local x = (1 + math.sin(n)) * txtMask:getWidth() / 2
  txtPost:copyPixels(txt)
  txtMask:clear(1, 1, 1, .5)
  txtMask:drawRect(x - 10, 0, 20, 100, 1, 1, 1, .6)
  txtMask:drawRect(x -  5, 0, 10, 100, 1, 1, 1, 1)
	sol.bufferfx.mask(txtPost, txtMask)
  local tx, ty = (height - txt:getWidth()) / 2, (height - txt:getHeight()) / 2
  sol.graphics.reset()
  sol.graphics.setColor(0, 0, 0, 1)
  for i = #stroke, 1, -1 do
    local x = (tx - 4) + (1 + stroke[i][1]) * 4
    local y = (ty - 4) + (1 + stroke[i][2]) * 4
    sol.graphics.draw(txt, x, y)
  end
  sol.graphics.reset()
	sol.graphics.draw(txtPost, tx, ty)
end

function sol.onKeyDown(k)
	if k == "escape" then
		sol.system.quit()
	end
end
