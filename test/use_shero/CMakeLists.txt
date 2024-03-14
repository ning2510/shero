cmake_minimum_required(VERSION 3.10)
project(example)

find_library(SHERO shero REQUIRED)
find_library(YAMLCPP yaml-cpp REQUIRED)
find_library(TINYXML tinyxml REQUIRED)
find_package(OpenSSL REQUIRED)

SET(LIBS
    ${SHERO}
    pthread
    dl
    ${YAMLCPP}
    ${TINYXML}
    ${OPENSSL_LIBRARIES}
)

add_executable(test_tcp_server test_tcp_server.cc)
target_link_libraries(test_tcp_server pthread ${LIBS})