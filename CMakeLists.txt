project(ZLToolKit)
cmake_minimum_required(VERSION 2.8)

#加载自定义模块
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
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

#非苹果平台移除.mm类型的文件
if(NOT APPLE)
    list(REMOVE_ITEM SRC_LIST "src/Network/Socket_ios.mm")
endif()


set(LINK_LIB_LIST)


set(ENABLE_OPENSSL false)
set(ENABLE_MYSQL false)



#打印库文件
message(STATUS "将链接依赖库:${LINK_LIB_LIST}")
#引用头文件路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
#使能c++11
add_compile_options(-std=c++11)

if(NOT WIN32)
    add_compile_options(-Wno-deprecated-declarations)
    add_compile_options(-Wno-predefined-identifier-outside-function)
endif()

#编译静态库
add_library(${CMAKE_PROJECT_NAME}_static STATIC ${SRC_LIST})
set_target_properties(${CMAKE_PROJECT_NAME}_static PROPERTIES OUTPUT_NAME "${CMAKE_PROJECT_NAME}")
#安装静态库至系统目录
install(TARGETS ${CMAKE_PROJECT_NAME}_static ARCHIVE DESTINATION ${INSTALL_PATH_LIB})


add_subdirectory(tests)

























