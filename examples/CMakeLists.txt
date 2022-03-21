

#set(CMAKE_FIND_DEBUG_MODE TRUE)

find_library(FMTLIB "fmt" ${LPG_GUI_DEPS_LIB})

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

set(LPG_GUI_EXAMPLE_LIBS 
    lpg-gui
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
    ${FMTLIB}
)

add_custom_target(lpg_gui_examples ALL)

add_custom_command(TARGET lpg_gui_examples POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/examples/resources/data/ ${CMAKE_CURRENT_BINARY_DIR}
)

if(WIN32)
    add_custom_command(TARGET lpg_gui_examples POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/examples/resources/dll/ ${CMAKE_CURRENT_BINARY_DIR}
    ) 
endif()

function(axxegro_add_example name)
    add_executable(${name} "src/${name}.cpp")
    add_dependencies(lpg_gui_examples ${name})
    target_link_libraries(${name} LINK_PUBLIC ${LPG_GUI_EXAMPLE_LIBS})
endfunction()


axxegro_add_example("example1")