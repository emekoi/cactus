local sqr
sqr = function(freq, amp)
  amp = amp or 1
  local tb = {
    freq = freq,
    amp = amp
  }
  local mtb = {
    __call = function(self, t)
      return ((self.freq * t) % 1) > .5 and self.amp or -self.amp
    end
  }
  return setmetatable(tb, mtb)
end
local tri
tri = function(freq, amp)
  amp = amp or 1
  local tb = {
    freq = freq,
    amp = amp
  }
  local mtb = {
    __call = function(self, t)
      t = (self.freq * t) % 1
      return t < .5 and self.amp * (4 * t - 1) or self.amp * (3 - 4 * t)
    end
  }
  return setmetatable(tb, mtb)
end
local saw
saw = function(freq, amp)
  amp = amp or 1
  local tb = {
    freq = freq,
    amp = amp
  }
  local mtb = {
    __call = function(self, t)
      t = (self.freq * t) % 1
      return self.amp * (2 * t - 1)
    end
  }
  return setmetatable(tb, mtb)
end
local sin
sin = function(freq, amp)
  amp = amp or 1
  local tb = {
    freq = freq,
    amp = amp
  }
  local mtb = {
    __call = function(self, t)
      t = (self.freq * t) % 1
      return self.amp * math.sin(2 * math.pi * t)
    end
  }
  return setmetatable(tb, mtb)
end
local wht
wht = function(amp)
  amp = amp or 1
  local tb = {
    amp = amp
  }
  local mtb = {
    __call = function(self)
      return self.amp * (math.random() * 2 - 1)
    end
  }
  return setmetatable(tb, mtb)
end
local pnk
pnk = function(amp)
  amp = amp or 1
  local tb = {
    amp = amp,
    last = 0
  }
  local mtb = {
    __call = function(self)
      self.last = math.max(-1, math.min(1, self.last + math.random() * 2 - 1))
      return self.amp * self.last
    end
  }
  return setmetatable(tb, mtb)
end
return {
  sqr = sqr,
  tri = tri,
  saw = saw,
  sin = sin,
  wht = wht,
  pnk = pnk
}
