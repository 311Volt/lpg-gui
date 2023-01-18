


add_custom_target(lpg_gui_examples ALL)

add_custom_command(TARGET lpg_gui_examples POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/examples/resources/data/ ${CMAKE_CURRENT_BINARY_DIR}
)

# if(WIN32)
#     add_custom_command(TARGET lpg_gui_examples POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_directory
#         ${CMAKE_SOURCE_DIR}/examples/resources/dll/ ${CMAKE_CURRENT_BINARY_DIR}
#     ) 
# endif()

function(lpg_gui_add_example name)
    add_executable("lpg_gui_ex_${name}" "src/${name}.cpp")
    add_dependencies("lpg_gui_ex_${name}" lpg_gui_examples)
    target_link_libraries("lpg_gui_ex_${name}" LINK_PUBLIC lpg_gui)
    
endfunction()


lpg_gui_add_example("example1")