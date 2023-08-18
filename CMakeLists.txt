cmake_minimum_required(VERSION 3.10)
project(shero)

SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include(cmake/utils.cmake)
include_directories(.)

option(BENCH_MARK "benchmark" OFF)
if(BENCH_MARK)
	add_definitions(-DBENCH_MARK)
endif()

SET(SRC_FILES
    shero/base/Log.cc
    shero/base/Util.cc
    shero/base/Timer.cc
    shero/base/Config.cc
    shero/base/Thread.cc
    shero/net/Buffer.cc
    shero/net/Socket.cc
    shero/net/Poller.cc
    shero/net/Address.cc
    shero/net/Channel.cc
    shero/net/EventLoop.cc
    shero/net/EPollPoller.cc
    shero/net/EventLoopThread.cc
    shero/net/EventLoopThreadPool.cc
    shero/net/tcp/TcpClient.cc
    shero/net/tcp/TcpServer.cc
    shero/net/tcp/TcpAcceptor.cc
    shero/net/tcp/TcpConnector.cc
    shero/net/tcp/TcpConnection.cc
    shero/net/http/HttpStatus.cc
    shero/net/http/HttpParser.cc
    shero/net/http/HttpServer.cc
    shero/net/http/HttpCommon.cc
    shero/net/http/HttpServlet.cc
    shero/net/http/HttpDispatch.cc
    shero/net/http/client/HttpClient.cc
    shero/net/http/client/HttpClientParser.cc
    shero/net/websocket/WSClient.cc
    shero/net/websocket/WSServer.cc
    shero/net/websocket/WSServlet.cc
    shero/net/websocket/WSDispatch.cc
    shero/net/websocket/WSStructure.cc
)

ragelmaker(shero/net/Uri.rl SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/shero/net)
ragelmaker(shero/net/http/client/httpclient_parser.rl SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/shero/net/http/client)

add_library(shero SHARED ${SRC_FILES})

find_library(YAMLCPP yaml-cpp REQUIRED)
find_library(TINYXML tinyxml REQUIRED)
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif()

SET(LIBS
    shero
    pthread
    dl
    ${YAMLCPP}
    ${TINYXML}
    ${OPENSSL_LIBRARIES}
)

add_subdirectory(test)