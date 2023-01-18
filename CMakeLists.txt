cmake_minimum_required(VERSION 3.8.0)
project(lpg_gui VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(NOT MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()


file(GLOB_RECURSE lpg_gui_sources "src/*.cpp")
add_library(lpg_gui STATIC ${lpg_gui_sources})


option(AXXEGRO_MASTER_PROJECT OFF)
add_subdirectory("axxegro")
find_package(lunasvg REQUIRED)


target_link_libraries(lpg_gui PUBLIC axxegro)
target_link_libraries(lpg_gui PRIVATE ${lunasvg_LIBRARIES})
target_include_directories(lpg_gui PRIVATE ${lunasvg_INCLUDE_DIRS})
target_include_directories(lpg_gui PUBLIC "include")


option(LPG_GUI_BUILD_EXAMPLES "Build example programs" ON)
if(LPG_GUI_BUILD_EXAMPLES)
	add_subdirectory("examples")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
