

function sol.onLoad()
	sol.graphics.setClearColor(0.5, 0.5, 0.5)
	img = sol.Buffer.fromString(sol.fs.read("cat.png"))
	-- width, height = 450/2, 370/2
	ticks, rot = 0, 0
	sol.debug.setVisible(true)
	font = sol.Font.fromEmbedded(48)
end

function sol.onDraw()
  ticks = ticks + 0.2
  rot = rot + 1
  sol.graphics.drawText(font, "hello WORLD", 0, 0)
  -- sol.graphics.drawBuffer(img, 0, 0, nil, math.rad(rot),  1, 1, width, height)
end
