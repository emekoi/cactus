function createCube(t)
  G.points = {}
  local l, p
  l = 5 + math.abs((math.sin(t / 100)) * 10)
  for x = - 3, 3 do
    for y = - 3, 3 do
      for z = - 3, 3 do
        p = { x = x * l, y = y * l, z = z * l, c = G.pal[4 + z] }
        table.insert(G.points, p)
      end
    end
  end
end

function createCurve(t)
  G.points = {}
  local i, l = 0
  l1 = math.abs(5 * (math.cos(t / 50)))
  for u = - 70, 70, 15 do
    local r1 = math.cos(math.rad(u)) * 30
    for v = 1, 360, 15 do
      local x = math.sin(math.rad(v))
      local y = math.cos(math.rad(v))
      p = { x = x * r1, y = y * r1, z = (u / (l1 + 0.7) + v / 5), c = G.pal[1 + i % 7] }
      table.insert(G.points, p)
    end
    i = i + 1
  end
end

function p2d(p3d)
  local fov = 180
  local x0 = p3d.x + G.cx
  local y0 = p3d.y + G.cy
  local z0 = p3d.z + G.cz
  local x2d = fov * x0 / z0
  local y2d = fov * y0 / z0
  x2d = x2d + G.width / 2
  y2d = y2d + G.height / 2
  return x2d, y2d
end

function rotate(p3d, center, ax, ay, az)
  local a, b, c
  local a1, b1, c1
  local a2, b2, c2
  local a3, b3, c3
  local np3d = { x = 0, y = 0, z = 0, c = 0 }
  a = p3d.x - center.x
  b = p3d.y - center.y
  c = p3d.z - center.z

  a1 = a * math.cos(az) - b * math.sin(az)
  b1 = a * math.sin(az) + b * math.cos(az)
  c1 = c

  c2 = c1 * math.cos(ay) - a1 * math.sin(ay)
  a2 = c1 * math.sin(ay) + a1 * math.cos(ay)
  b2 = b1

  b3 = b2 * math.cos(ax) - c2 * math.sin(ax)
  c3 = b2 * math.sin(ax) + c2 * math.cos(ax)
  a3 = a2
  np3d.x = a3
  np3d.y = b3
  np3d.z = c3
  np3d.c = p3d.c
  return np3d
end

function zsort(p1, p2)
  return p1.z > p2.z
end

function sol.onLoad()
  G.buffer = sol.Buffer.fromBlank(G.width, G.height)
end

function sol.onUpdate(dt)
  if sol.keyboard.isDown("left") then G.cx = G.cx + 2 end
  if sol.keyboard.isDown("right") then G.cx = G.cx - 2 end
  if sol.keyboard.isDown("up") then G.cy = G.cy + 2 end
  if sol.keyboard.isDown("down") then G.cy = G.cy - 2 end
  if sol.keyboard.isDown("x") then G.cz = G.cz + 2 end
  if sol.keyboard.isDown("c") then G.cz = G.cz - 2 end
  if G.mode ==  "play" then
    if (G.t % 900 > 450) then
      createCurve(G.t)
    else
      createCube(G.t)
    end
    for k, p in pairs(G.points) do
      pr = rotate(p, G.pivot, G.angle, G.angle / 2, G.angle / 4)
      G.points[k] = pr
    end
    table.sort(G.points, zsort)
    G.t = G.t + 1
  end
end

function sol.onDraw()
  for k, p in pairs(G.points) do
    i, j = p2d(p)
    G.buffer:drawRect(i, j, 4, 4, unpack(colors[p.c]))
  end
  G.angle = G.angle + 0.05
  sol.graphics.copyPixels(G.buffer, 0, 0, nil, G.scale)
  sol.graphics.drawText(sol.Font.fromEmbedded(16), string.format("(%d, %d, %d)", G.cx, G.cy, G.cz), 10, 10)
  sol.graphics.drawText(sol.Font.fromEmbedded(16), G.t, 10, 30)
  G.buffer:clear(unpack(colors[10]))
end

function sol.onKeyDown(k)
  if k ==  "p" and G.mode == "play" then
    G.mode = "paused"
  elseif k == "p" and G.mode == "paused" then
    G.mode = "play"
  end
  if k ==  "escape" then os.exit() end
end
