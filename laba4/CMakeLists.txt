
cmake_minimum_required(VERSION 3.10)
project(DynamicLab C)


set(CMAKE_C_STANDARD 11)


include_directories(${CMAKE_SOURCE_DIR}/contracts)
include_directories(${CMAKE_SOURCE_DIR}/lib_impl1)
include_directories(${CMAKE_SOURCE_DIR}/lib_impl2)


add_library(impl1 SHARED
lib_impl1/impl1.c
)
set_target_properties(impl1 PROPERTIES OUTPUT_NAME "impl1")


add_library(impl2 SHARED
lib_impl2/impl2.c
)
set_target_properties(impl2 PROPERTIES OUTPUT_NAME "impl2")


add_executable(program1_static program1_static/main_static.c)
target_link_libraries(program1_static impl1)


add_executable(program2_dynamic program2_dynamic/main_dynamic.c)