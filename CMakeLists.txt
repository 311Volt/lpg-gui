cmake_minimum_required(VERSION 3.8.0)
project(lpg-gui VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-m32")

include_directories("include")
include_directories("deps/include")

file(GLOB_RECURSE lpg_gui_sources "src/*.cpp")

set(CMAKE_VERBOSE_MAKEFILE ON)
file(REAL_PATH "deps/lib" LPG_GUI_DEPS_LIB)

add_library(lpg-gui ${lpg_gui_sources})
find_library(LIBALLEG allegro_monolith.dll ${LPG_GUI_DEPS_LIB})
find_library(LIBFMT fmt ${LPG_GUI_DEPS_LIB})
find_library(LIBLUNASVG lunasvg ${LPG_GUI_DEPS_LIB})

#option(LPG_GUI_BUILD_EXAMPLES ON)
#if(LPG_GUI_BUILD_EXAMPLES)
	add_subdirectory("examples")
#endif()

target_link_libraries(lpg-gui ${LIBALLEG} ${LIBFMT} ${LIBLUNASVG})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)