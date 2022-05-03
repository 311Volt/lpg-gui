cmake_minimum_required(VERSION 3.8.0)
project(lpg_gui VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_VERBOSE_MAKEFILE TRUE)

if(WIN32)
	set(CMAKE_C_FLAGS "-m32")
	set(CMAKE_CXX_FLAGS "-m32 -Wall -Wextra -pedantic")
endif()

include_directories("include")
include_directories("deps/include")
include_directories("axxegro/include")
include_directories("axxegro/fmt/include")
include_directories("axxegro/tcbspan/include")

include_directories("lunasvg/include")

file(GLOB_RECURSE lpg_gui_sources "src/*.cpp")

file(REAL_PATH "deps/lib" LPG_GUI_DEPS_LIB)

add_library(lpg_gui STATIC ${lpg_gui_sources})


# set(CMAKE_FIND_DEBUG_MODE TRUE)

# find_library(LIBFMT fmt ${LPG_GUI_DEPS_LIB})

find_library(ALLEGRO_CORE "allegro" ${LPG_GUI_DEPS_LIB})
find_library(ALLEGRO_MAIN "allegro_main" ${LPG_GUI_DEPS_LIB})
find_library(ALLEGRO_ACODEC "allegro_acodec" ${LPG_GUI_DEPS_LIB})
find_library(ALLEGRO_AUDIO "allegro_audio" ${LPG_GUI_DEPS_LIB})
find_library(ALLEGRO_COLOR "allegro_color" ${LPG_GUI_DEPS_LIB})
find_library(ALLEGRO_DIALOG "allegro_dialog" ${LPG_GUI_DEPS_LIB})
find_library(ALLEGRO_FONT "allegro_font" ${LPG_GUI_DEPS_LIB})
find_library(ALLEGRO_IMAGE "allegro_image" ${LPG_GUI_DEPS_LIB})
find_library(ALLEGRO_MEMFILE "allegro_memfile" ${LPG_GUI_DEPS_LIB})
find_library(ALLEGRO_PHYSFS "allegro_physfs" ${LPG_GUI_DEPS_LIB})
find_library(ALLEGRO_PRIMITIVES "allegro_primitives" ${LPG_GUI_DEPS_LIB})
find_library(ALLEGRO_TTF "allegro_ttf" ${LPG_GUI_DEPS_LIB})
find_library(ALLEGRO_VIDEO "allegro_video" ${LPG_GUI_DEPS_LIB})


OPTION(AXXEGRO_BUILD_EXAMPLES "" OFF)
add_subdirectory("axxegro")
add_subdirectory("lunasvg")

set(LPG_GUI_LIBS 
    axxegro
    fmt
    lunasvg
    ${ALLEGRO_CORE} 
    ${ALLEGRO_MAIN} 
    ${ALLEGRO_ACODEC} 
    ${ALLEGRO_AUDIO} 
    ${ALLEGRO_COLOR} 
    ${ALLEGRO_DIALOG} 
    ${ALLEGRO_FONT} 
    ${ALLEGRO_IMAGE} 
    ${ALLEGRO_MEMFILE} 
    ${ALLEGRO_PHYSFS} 
    ${ALLEGRO_PRIMITIVES} 
    ${ALLEGRO_TTF} 
    ${ALLEGRO_VIDEO}
)



target_link_libraries(lpg_gui PUBLIC ${LPG_GUI_LIBS})

option(LPG_GUI_BUILD_EXAMPLES "Build example programs" ON)
if(LPG_GUI_BUILD_EXAMPLES)
	add_subdirectory("examples")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
