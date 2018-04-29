local _ = require "lume"

--[[
  "brown"     -> { "dark-purple" }
  "light-grey"-> { "dark-grey"   }
  "white"     -> { "peach"       }
  "red"       -> { "dark-purple" }
  "orange"    -> { "brown"       }
  "yellow"    -> { "orange"      }
  "green"     -> { "dark-green"  }
  "blue"      -> { "indigo"      }
  "indigo"    -> { "dark-blue"   }
]]

return {
  {
    ["black"]       = { _.color("#000000", 1) },
    ["dark-blue"]   = { _.color("#1d2b53", 1) },
    ["dark-purple"] = { _.color("#7e2553", 1) },
    ["dark-green"]  = { _.color("#008751", 1) },
    ["brown"]       = { _.color("#ab5236", 1) },
    ["dark-grey"]   = { _.color("#5f574f", 1) },
    ["light-grey"]  = { _.color("#c2c3c7", 1) },
    ["white"]       = { _.color("#fff1e8", 1) },
    ["red"]         = { _.color("#ff004d", 1) },
    ["orange"]      = { _.color("#ffa300", 1) },
    ["yellow"]      = { _.color("#ffec27", 1) },
    ["green"]       = { _.color("#00e436", 1) },
    ["blue"]        = { _.color("#29adff", 1) },
    ["indigo"]      = { _.color("#83769c", 1) },
    ["pink"]        = { _.color("#ff77a8", 1) },
    ["peach"]       = { _.color("#ffccaa", 1) },
  }, {
    { _.color("#000000", 1) },
    { _.color("#1d2b53", 1) },
    { _.color("#7e2553", 1) },
    { _.color("#008751", 1) },
    { _.color("#ab5236", 1) },
    { _.color("#5f574f", 1) },
    { _.color("#c2c3c7", 1) },
    { _.color("#fff1e8", 1) },
    { _.color("#ff004d", 1) },
    { _.color("#ffa300", 1) },
    { _.color("#ffec27", 1) },
    { _.color("#00e436", 1) },
    { _.color("#29adff", 1) },
    { _.color("#83769c", 1) },
    { _.color("#ff77a8", 1) },
    { _.color("#ffccaa", 1) },
  }
}
