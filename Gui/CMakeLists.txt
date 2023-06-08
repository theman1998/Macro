set(${PROJECT_NAME} MacroGui2)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories(${CMAKE_SOURCE_DIR}/use_all)

set(PROJECT_SOURCES
        "main.cpp"
        "MacroWindow.cpp"
)

set(PROJECT_HEADERS
    "MacroWindow.h"
)

SET(PROJECT_UI 
    "MacroWindow.ui"
)

set(AUTO_GEN_DIR "${PROJECT_BINARY_DIR}/MacrGui_autogen/include")


#include_directories (AUTO_GEN_DIR)
message( "---------- include_directories (${PROJECT_BINARY_DIR}) ------------------------")


find_package(Qt6 REQUIRED COMPONENTS Widgets Core)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
    ${PROJECT_UI}
)


if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
    target_compile_options( ${PROJECT_NAME} PUBLIC "/Zc:__cplusplus")
    target_compile_options( ${PROJECT_NAME} PUBLIC /permissive-)
endif()

#add_library(Qt6::Widgets STATIC IMPORTED)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Widgets Qt6::Core use_all)


#target_link_libraries(${PROJECT_NAME} use_all)

#set_target_properties(${PROJECT_NAME} PROPERTIES
#    Qt6
#)
#WIN32_EXECUTABLE ON

message( "Stating command ${PATH_TO_DELAY_QT} ${CMAKE_BINARY_DIR}")
execute_process(COMMAND ${PATH_TO_DELAY_QT} ${CMAKE_BINARY_DIR})
