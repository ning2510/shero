cmake_minimum_required(VERSION 3.10)
project(shero)

enable_language(ASM)
SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include(cmake/utils.cmake)
include_directories(.)

SET(SRC_FILES
    shero/base/Log.cc
    shero/base/Util.cc
    shero/base/Timer.cc
    shero/base/Config.cc
    shero/base/Thread.cc
    shero/base/Buffer.cc
    shero/coroutine/Hook.cc
    shero/coroutine/Memory.cc
    shero/coroutine/Coroutine.cc
    shero/coroutine/CoroutinePool.cc
    shero/net/Socket.cc
    shero/net/Poller.cc
    shero/net/Address.cc
    shero/net/Channel.cc
    shero/net/EventLoop.cc
    shero/net/EPollPoller.cc
    shero/net/EventLoopThread.cc
    shero/net/EventLoopThreadPool.cc
    shero/net/http/Http.cc
    shero/net/http/HttpStatus.cc
    shero/net/http/HttpParser.cc
    shero/net/tcp/TcpClient.cc
    shero/net/tcp/TcpServer.cc
    shero/net/tcp/TcpAcceptor.cc
    shero/net/tcp/TcpConnector.cc
    shero/net/tcp/TcpConnection.cc
)

ragelmaker(shero/net/http/http11_parser.rl SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/shero/net/http)
ragelmaker(shero/net/http/httpclient_parser.rl SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/shero/net/http)

SET(COXTX shero/coroutine/coctx_swap.S)
add_library(shero SHARED ${SRC_FILES} ${COXTX})

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