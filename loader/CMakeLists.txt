#
# What does this part actually do?
#
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