#!/usr/bin/python2.7
import os, sys, shutil, platform, time
import cembed

OUTPUT = "bin/cactus"
EMBED_DIR = "src/embed"
TEMPSRC_DIR = ".tempsrc"
COMPILER = "gcc"
INCLUDE = [
  TEMPSRC_DIR,
  "src",
  "src/module",
  "src/lib/wren",
  "src/lib/wren/vm",
  "src/lib/wren/optional",
]
SOURCE = [
  "src/*.c",
  "src/module/*.c",
  "src/lib/vec/*.c",
  "src/lib/map/*.c",
  "src/lib/sera/*.c",
  "src/lib/stb_vorbis.c",
]
WREN = [
  "src/lib/wren/vm/*.c",
  "src/lib/wren/optional/*.c",
]
# FLAGS = [ "-Wall", "-Wextra", "--std=c99", "-fno-strict-aliasing", "-Wno-unused-value", "-Wno-misleading-indentation" ]
FLAGS = [ "-Wall", "-Wextra", "--std=c99", "-fno-strict-aliasing", "-Wno-unused-value" ]
# FLAGS = [ "--std=c99", "-fno-strict-aliasing" ]
LINK = [ "m" ]
LINKER = [  ]
DEFINE = [ "WREN_OPT_META", "WREN_OPT_META" ]
EXTRA = [  ]

if platform.system() == "Windows":
  sys.stdout = os.fdopen(sys.stdout.fileno(), 'w', 0)
  OUTPUT += ".exe"
  LINK += [ "mingw32", "SDL2main", "SDL2" ]
  # FLAGS += [ "-mwindows" ]


if platform.system() == "Linux":
  LINK += [ "SD2Lmain", "SDL2" ]
  DEFINE += [ "SR_MODE_ABGR" ]


if platform.system() == "Darwin":
  FLAGS += [ os.popen("sdl2-config --cflags").read().strip() ]
  EXTRA += [ os.popen("sdl2-config --libs").read().strip() ]
  # DEFINE += [ "SR_MODE_ARGB" ]


def fmt(fmt, dic):
  for k in dic:
    fmt = fmt.replace("{" + k + "}", str(dic[k]))
  return fmt


def clearup():
  if os.path.exists(TEMPSRC_DIR):
    shutil.rmtree(TEMPSRC_DIR)


def main():
  global FLAGS, LINK, DEFINE

  print "initing..."
  starttime = time.time()

  # Handle args
  build = "release" if "release" in sys.argv else "debug"
  verbose = "verbose" in sys.argv

  # Handle build type
  if build == "debug":
    FLAGS += [ "-g"  ]
  else:
    FLAGS += [ "-O3" ]
    if platform.system() == "Windows":
      FLAGS += [ "-mwindows" ]
    DEFINE += [ "CACTUS_RELEASE" ]

  # Handle "nojit" option -- compile with normal embedded Lua instead

  print "building (" + build + ")..."

  # Make sure there arn't any temp files left over from a previous build
  clearup()

  # Make sure the previous binary is deleted (windows)
  if os.path.isfile(OUTPUT):
    os.remove(OUTPUT)

  # Create directories
  os.makedirs(TEMPSRC_DIR)
  outdir = os.path.dirname(OUTPUT)
  if not os.path.exists(outdir):
    os.makedirs(outdir)

  # Create embedded-file header files
  for filename in os.listdir(EMBED_DIR):
    fullname = EMBED_DIR + "/" + filename
    res = cembed.process(fullname)
    open(TEMPSRC_DIR + "/" + cembed.safename(fullname) + ".h", "wb").write(res)

  # Build
  cmd = fmt(
    "{compiler} -o {output} {flags} {source} {wren} {include} {linker} {link} {define} " +
    "{extra}",
    {
      "compiler"  : COMPILER,
      "output"    : OUTPUT,
      "source"    : " ".join(SOURCE),
      "wren"      : " ".join(WREN),
      "include"   : " ".join(map(lambda x:"-I" + x, INCLUDE)),
      "linker"    : " ".join(map(lambda x:"-L" + x, LINKER)),
      "link"      : " ".join(map(lambda x:"-l" + x, LINK)),
      "define"    : " ".join(map(lambda x:"-D" + x, DEFINE)),
      "flags"     : " ".join(FLAGS),
      "extra"     : " ".join(EXTRA),
    })

  if verbose:
    print cmd

  print "compiling..."
  res = os.system(cmd)

  if build == "release":
    print "stripping..."
    os.system("strip %s" % OUTPUT)

  print "clearing up..."
  clearup()

  if res == 0:
    print "done (%.2fs)" % (time.time() - starttime)
  else:
    print "done with errors"
  sys.exit(res)


if __name__ == "__main__":
  main()
