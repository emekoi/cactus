

function sol.onLoad(dt)
  math.randomseed(sol.time.getNow())
  sol.debug.setVisible(true)
  G.screen = sol.Buffer.fromBlank(G.width, G.height)
  G.particle = sol.Buffer.fromFile("data/image/particle.png")
  G.particles = {}
  for i = 0, 200 do
    table.insert(G.particles, {
      x = 0,
      y = 0,
      vx = 0,
      vy = 0,
      t = 0,
      a = 0,
      s = math.random(),
    })
  end
end


function sol.onUpdate(dt)
  if sol.keyboard.wasPressed("escape") then sol.system.quit() end
  if sol.keyboard.wasPressed("`") then sol.debug.setFocused(not sol.debug.getFocused()) end
  for i, p in ipairs(G.particles) do
    p.x = p.x + p.vx * dt
    p.y = p.y + p.vy * dt
    if p.t > 0 then
      p.t = p.t - dt
    else
      p.a = p.a - dt * 3
      if p.a < 0 then
        local r = math.random() * math.pi * 2
        p.x = math.cos(r) * 20
        p.y = math.sin(r) * 20
        p.vx = (1 - math.random() * 2) * 90
        p.vy = (1 - math.random() * 2) * 90
        p.t = math.random() * 1
        p.a = 1
      end
    end
  end
end


function sol.onDraw()
  G.screen:clear(0,0,0,1)
  G.screen:setBlend("add")
  G.screen:setColor(.2, .4, 1)
  for i, p in ipairs(G.particles) do
    G.screen:setAlpha(p.a)
    G.screen:draw(G.particle, G.width / 2 + p.x, G.height / 2 + p.y,
                  nil, 0, p.s, p.s, 16, 16)
  end
  sol.bufferfx.dissolve(G.screen,math.random())
  sol.graphics.draw(G.screen, 0, 0, nil, nil, G.scale)
end
