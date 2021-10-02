add_library(server INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/gamedll.cpp
	${CMAKE_CURRENT_LIST_DIR}/loader.cpp
	${CMAKE_CURRENT_LIST_DIR}/serverplugin.cpp
	${CMAKE_CURRENT_LIST_DIR}/utility.cpp
)

target_include_directories(server INTERFACE
	${CMAKE_SOURCE_DIR}/public/sourcehook
)

target_link_libraries(server INTERFACE sdk_wrapper)

#[[
if (NOT WIN32)
	if (TARGET_ARCH MATCHES "x86")
		set_target_properties(server PROPERTIES PREFIX "lib" SUFFIX "${HL2SDK_LIB_SHARED_EXT}")
	elseif (TARGET_ARCH MATCHES "x64")
		set_target_properties(server PROPERTIES PREFIX "lib" SUFFIX "_i486${HL2SDK_LIB_SHARED_EXT}")
	endif()
endif()
]]