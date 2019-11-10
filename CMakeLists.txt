cmake_minimum_required(VERSION 2.8.9)

set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
project (leetcode_c-)

#include_directories(/usr/include/python3.5m)
include_directories(include)
file(GLOB SOURCES "src/*.cpp")

include(cmake/modules/Findlibdb_cxx.cmake)
#判断编译器类型,如果是gcc编译器,则在编译选项中加入c++11支持 
if(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-std=c++11)
    message(STATUS "optional:-std=c++11")   
endif(CMAKE_COMPILER_IS_GNUCXX)

add_compile_options(-std=c++11)
#查找当前目录下的所有源文件
#并将名称保存到DIR_SRCS变量中
aux_source_directory(. DIR_SRCS)
#为源文件的编译添加选项
#ADD_DEFINITIONS(-g -W -Wall -o2))
link_directories(./lib)
message(STATUS "add exectable")

set(CMAKE_SOURCE_DIR ..)
set(CMAKE_MODULE_PATH ${CMAKE_ROOT}/Modules  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
find_package(libdb_cxx REQUIRED)
#mark_as_advanced(
#    LIBDB_CXX_INCLUDE_DIR
#    LIBDB_CXX_LIBRARIES
#)
if(LIBDB_CXX_INCLUDE_DIR AND LIBDB_CXX_LIBRARIES)
    message(STATUS "Found libdb libraries and include")
endif()
if(LIBDB_CXX_FOUND)
#if(libdb_cxx_FOUND)
    message(STATUS "Found libdb libraries and include")
endif()
#得出可执行文件
add_executable(leetcode_c-  ${SOURCES})
#link libray in static mode
target_link_libraries(leetcode_c-)
