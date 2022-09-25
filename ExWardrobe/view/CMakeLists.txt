project(ExWardrobe)
set(QT_DIR "D:/Qt/6.4.0/msvc2019_64/lib/cmake")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets LinguistTools)

file(GLOB SRC_FILES "./*.cpp")

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    qt_add_executable(${PROJECT_NAME} WIN32 MANUAL_FINALIZATION ${SRC_FILES})
ELSE()
    qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${SRC_FILES})
ENDIF()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets core)

qt_finalize_executable(${PROJECT_NAME})