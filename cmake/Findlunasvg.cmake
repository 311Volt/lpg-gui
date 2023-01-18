find_path(LUNASVG_INCLUDE_DIR lunasvg.h)
find_library(LUNASVG_LIBRARY lunasvg)

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args(lunasvg
	DEFAULT_MSG
	LUNASVG_INCLUDE_DIR
	LUNASVG_LIBRARY
)

mark_as_advanced(LUNASVG_LIBRARY LUNASVG_INCLUDE_DIR)

if(lunasvg_FOUND)
	set(lunasvg_LIBRARIES    ${LUNASVG_LIBRARY})
	set(lunasvg_INCLUDE_DIRS ${LUNASVG_INCLUDE_DIR})
endif()