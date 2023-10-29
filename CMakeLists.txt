#cmake所需的最小版本
cmake_minimum_required(VERSION 3.0)

#项目工程名称
project(mprpc)

# 生成debug版本，可以进行gdb调试
set(CMAKE_BUILD_TYPE "Debug")

#设置最终可执行文件的输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#设置最终库文件所输出的路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#设置头文件的搜索路径 -I
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)

#设置库文件的搜索路径 -L
link_directories(${PROJECT_SOURCE_DIR}/lib)

#加载子目录
add_subdirectory(src)
add_subdirectory(example)
