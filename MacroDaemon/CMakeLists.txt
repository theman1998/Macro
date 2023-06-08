set(PROJECT_NAME MacroDaemon)

set(${PROJECT_NAME}_SOURCES
    main.cpp
    GlobalData.h
    Controller.h
    Controller.cpp
 )

include_directories(".") # So we can include sub folders 
include_directories(${CMAKE_SOURCE_DIR}/use_all)

add_executable(${PROJECT_NAME}
    ${${PROJECT_NAME}_SOURCES}
)





if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
    target_compile_options( ${PROJECT_NAME} PUBLIC "/Zc:__cplusplus")
    target_compile_options( ${PROJECT_NAME} PUBLIC /permissive-)
    target_link_options( ${PROJECT_NAME} PUBLIC "/INCREMENTAL")
endif()


target_link_libraries(${PROJECT_NAME} PUBLIC use_all)