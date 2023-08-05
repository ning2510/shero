cmake_minimum_required(VERSION 3.10)
project(shero)

SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(.)

SET(SRC_FILES
    shero/base/Log.cc
    shero/base/Util.cc
    shero/base/Timer.cc
    shero/base/Config.cc
    shero/base/Thread.cc
    shero/base/Buffer.cc
    shero/coroutine/Hook.cc
    shero/net/Socket.cc
    shero/net/Poller.cc
    shero/net/Address.cc
    shero/net/Channel.cc
    shero/net/EventLoop.cc
    shero/net/EPollPoller.cc
)
add_library(shero SHARED ${SRC_FILES})

find_library(YAMLCPP yaml-cpp REQUIRED)
find_library(TINYXML tinyxml REQUIRED)

SET(LIBS
    shero
    pthread
    dl
    ${YAMLCPP}
    ${TINYXML}
)

add_subdirectory(test)