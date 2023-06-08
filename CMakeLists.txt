cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME MacroSoftware)



project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PATH_TO_DELAY_QT "C:\\Qt\\6.5.0\\msvc2019_64\\bin\\windeployqt.exe")
list(APPEND CMAKE_PREFIX_PATH "C:\\Qt\\6.5.0\\msvc2019_64")


#find_package(QT NAMES Qt REQUIRED COMPONENTS Widgets)

message( "${CMAKE_PREFIX_PATH}")
message( "Qt${QT_VERSION_MAJOR} ------------------------------------------------------------- ${PATH_TO_DELAY_QT}")
message(STATUS "Build directory: ${CMAKE_BINARY_DIR}/include")

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)
qt_standard_project_setup()


add_subdirectory(Gui)
add_subdirectory(use_all)
add_subdirectory(MacroDaemon)





