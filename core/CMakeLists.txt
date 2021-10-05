set(METAMOD_FILES 
	${CMAKE_CURRENT_LIST_DIR}/metamod.cpp
	${CMAKE_CURRENT_LIST_DIR}/metamod_console.cpp
	${CMAKE_CURRENT_LIST_DIR}/metamod_oslink.cpp
	${CMAKE_CURRENT_LIST_DIR}/metamod_plugins.cpp
	${CMAKE_CURRENT_LIST_DIR}/metamod_util.cpp
	${CMAKE_CURRENT_LIST_DIR}/provider/console.cpp
	${CMAKE_CURRENT_LIST_DIR}/provider/provider_ep2.cpp
)

set(SOURCEHOOK_FILES
	${CMAKE_SOURCE_DIR}/public/sourcehook/sourcehook.cpp
	${CMAKE_SOURCE_DIR}/public/sourcehook/sourcehook_impl_chookidman.cpp
	${CMAKE_SOURCE_DIR}/public/sourcehook/sourcehook_impl_chookmaninfo.cpp
	${CMAKE_SOURCE_DIR}/public/sourcehook/sourcehook_impl_cproto.cpp
	${CMAKE_SOURCE_DIR}/public/sourcehook/sourcehook_impl_cvfnptr.cpp
)

if (TARGET_ARCH MATCHES "x86")
	set(SOURCEHOOK_FILES
		${SOURCEHOOK_FILES}
		${CMAKE_SOURCE_DIR}/public/sourcehook/sourcehook_hookmangen.cpp
	)
endif()

add_library(metamod SHARED
	${METAMOD_FILES}
	${SOURCEHOOK_FILES}
)

target_include_directories(metamod PUBLIC 
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/provider
	${CMAKE_SOURCE_DIR}/loader
)

target_include_directories(metamod PUBLIC
	${CMAKE_SOURCE_DIR}/public
	${CMAKE_SOURCE_DIR}/public/sourcehook
)

target_link_libraries(metamod PUBLIC sdk_wrapper)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(metamod PRIVATE -Wno-format-truncation)
endif()

set_target_properties(metamod PROPERTIES PREFIX "" SUFFIX ".2.${HL2SDK_ENGINE_NAME}${HL2SDK_LIB_SHARED_EXT}")