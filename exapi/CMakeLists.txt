project(exapi LANGUAGES C)
file(GLOB SRC_FILES "./*.c")
include_directories("../zlib")

add_compile_definitions(EXAPI_EXPORTS)

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

find_library(LIBZLIB zlib "../zlib")
target_link_libraries(${PROJECT_NAME} ${LIBZLIB})