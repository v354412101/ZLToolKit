project(ZLToolKit)
cmake_minimum_required(VERSION 2.8)

#设置库文件路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
#设置可执行程序路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#设置子目录
set(SUB_DIR_LIST "Network" "Poller" "Thread" "Util")

set(INSTALL_PATH_LIB lib)
set(INSTALL_PATH_INCLUDE include)

foreach(SUB_DIR ${SUB_DIR_LIST})
    #遍历源文件
    aux_source_directory(src/${SUB_DIR} SRC_LIST)
    #安装头文件至系统目录
    install(DIRECTORY src/${SUB_DIR} DESTINATION ${INSTALL_PATH_INCLUDE} FILES_MATCHING PATTERN "*.h")
endforeach()

set(LINK_LIB_LIST)


set(ENABLE_OPENSSL false)
set(ENABLE_MYSQL false)

message(STATUS "library:${LINK_LIB_LIST}")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_compile_options(-std=c++11)

add_library(${CMAKE_PROJECT_NAME}_static STATIC ${SRC_LIST})
set_target_properties(${CMAKE_PROJECT_NAME}_static PROPERTIES OUTPUT_NAME "${CMAKE_PROJECT_NAME}")

install(TARGETS ${CMAKE_PROJECT_NAME}_static ARCHIVE DESTINATION ${INSTALL_PATH_LIB})

add_subdirectory(tests)

























