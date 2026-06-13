# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

idf_component_register(
    SRCS "main.cpp" "drivers/uart_console.cpp"
    INCLUDE_DIRS "." "${CMAKE_SOURCE_DIR}/include"
    REQUIRES driver freertos
)
