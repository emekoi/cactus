local osc = require "osc"

function set_audio_callback(wave)
  local delay = { idx = 0, max = 44100 * .4 }
  local phase = 0
  local xfreq = 0
  local xgain = 0
  local gen
  if wave == "wht" or wave == "pnk" then
    gen = osc[wave](1.5)
  else
    gen = osc[wave](math.random(550), 1)
  end
  sol.audio.master:setCallback(function(t)
    -- Clear visualisation table
    for i in ipairs(vis) do
      vis[i] = nil
    end
    -- Process audio
    for i = 1, #t, 2 do
      local dt = 1 / 44100
      -- Interp freq and gain (to avoid audible "steps" on high buffer size)
      xfreq = xfreq + (freq - xfreq) * .005
      xgain = xgain + (gain - xgain) * .005
      -- Increment phase
      phase = phase + dt * xfreq
      -- Generate sinewave
      -- local out = math.sin(phase * math.pi * 2)
      local out = gen(phase)
      -- Apply gain
      out = out * (xgain * .5) * vol
      -- Process delay
      out = out + (delay[delay.idx] or 0) * .5
      delay[delay.idx] = out
      delay.idx = (delay.idx + 1) % delay.max
      -- Write visualisation
      table.insert(vis, out)
      -- Write output
      t[i], t[i + 1] = out, out
    end
  end)
end

function sol.onLoad()
  sol.debug.setVisible(true)
  vis = {}
  freq = 0
  gain = 0
  sol.debug.addIndicator(function()
    return "freq",freq
  end)
  sol.debug.addIndicator(function()
    return "gain",gain
  end)
  set_audio_callback("sin")
end

function sol.onUpdate(dt)
  if sol.keyboard.isDown("right") then
    vol = vol + .1
  elseif sol.keyboard.isDown("left") then
    vol = math.max(0, vol - .1)
  end
end

function sol.onMouseMove(x, y)
  gain = (math.pow(1 - (y / sol.graphics.getWidth()), 1.8))
  freq = math.pow(x / sol.graphics.getHeight(), 2) * 3000 + 120
end

function sol.onKeyDown(key)
  key = tonumber(key)
  if key == 1 then
    set_audio_callback("sin")
  elseif key == 2 then
    set_audio_callback("tri")
  elseif key == 3 then
    set_audio_callback("saw")
  elseif key == 4 then
    set_audio_callback("sqr")
  elseif key == 5 then
    set_audio_callback("wht")
  elseif key == 6 then
    set_audio_callback("pnk")
  end
end


function sol.onDraw()
  -- Draw waveform
  sol.graphics.setColor(.7, .7, .7)
  local w, h = sol.graphics.getSize()
  local lastx, lasty
  for i, v in ipairs(vis) do
    local x, y = (i / #vis) * w, h / 2 + v * 100
    if i ~= 1 then
      sol.graphics.drawLine(lastx, lasty, x, y)
    end
    lastx, lasty = x, y
  end
  -- Draw x/y lines and info text
  sol.graphics.reset()
  local x, y = sol.mouse.getPosition()
  sol.graphics.drawLine(x, 0, x, sol.graphics.getHeight())
  sol.graphics.drawLine(0, y, sol.graphics.getWidth(), y)
  sol.graphics.drawText(string.format("freq: %.2fhz\ngain: %.2f",
                                       freq, gain), x + 10, y + 10)
end
