if (SOURCE2)
else()
include(${CMAKE_CURRENT_LIST_DIR}/Source1/ConfigureSource1.cmake)
endif()

#[[
#define MMS_BUILD_TAG		"manual"
#define MMS_BUILD_LOCAL_REV	"0"
#define MMS_BUILD_CSET		"0"
#define MMS_BUILD_MAJOR		"1"
#define MMS_BUILD_MINOR		"12"
#define MMS_BUILD_RELEASE	"0"

]]

find_package(Git)
if (GIT_FOUND)

	execute_process(
		COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		OUTPUT_VARIABLE MMS_BUILD_CSET
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE MMS_BUILD_LOCAL_REV
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif()
set(MMS_BUILD_TAG "dev" CACHE INTERNAL "Build tag")
set(MMS_BUILD_MAJOR 1 CACHE INTERNAL "Major version")
set(MMS_BUILD_MINOR 12 CACHE INTERNAL "Minor version")
set(MMS_BUILD_RELEASE 0 CACHE INTERNAL "Release version")

configure_file(
	${CMAKE_CURRENT_LIST_DIR}/configure.cmake.in
	${CMAKE_SOURCE_DIR}/public/metamod_version.h
)