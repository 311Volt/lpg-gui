# lpg-gui

A general-purpose, hardware-accelerated, retained-mode GUI library built with 
C++20 and [axxegro](https://github.com/311Volt/axxegro)
that can be used for games as well as other applications.

## future design goals

 - full hardware acceleration while still providing fallbacks for older machines
 - support for vector graphics and full scalability for any pixel density
 - looks exactly the same on every compatible platform
 - maximum support for themeability - in particular, make it as easy as possible
   to make fancy non-flat themes: it's been over a decade, it's time for 
   the ugly and braincell-destroying flat design to die already
 - high performance

## features so far
 - windows and basic controls
 - vector graphics
 - scalability
 - lots of bugs and bad code

# how to build
 - clone this repo with submodules
 - install [allegro5](https://github.com/liballeg/allegro5) and [lunasvg](https://github.com/sammycage/lunasvg)
   to your toolchain or `CMAKE_PREFIX_PATH` 
 - run CMake