# lpg-gui

A general-purpose, hardware-accelerated, retained-mode GUI library built with 
C++17 and [axxegro](https://github.com/311Volt/axxegro)
that can be used for games as well as other applications.

## design goals

 - full hardware acceleration while still providing fallbacks for ancient machines
 - support for vector graphics and full scalability for any pixel density
 - looks exactly the same, pixel for pixel, on every compatible platform
 - maximum support for themeability - in particular, make it as easy as possible
   to make fancy non-flat themes: it's been over a decade, it's time for 
   the ugly and braincell-destroying flat design to die already
 - high performance, obviously

## progress

you're never gonna guess: it's in super early development lol

however, i really care about getting this project done and develop it
every chance i get, so... come back in a year

# how to build
 - install/build allegro5 and axxegro
 - copy things to `deps/lib` and `deps/include` as needed
 - run cmake and then make