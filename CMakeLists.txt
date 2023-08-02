cmake_minimum_required(VERSION 3.10)
project(shero)

SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(.)

SET(SRC_FILES
    shero/util.cc
)

add_library(shero SHARED ${SRC_FILES})
SET(LIBS
    shero
    pthread
)

add_executable(test_mutex test/test_mutex.cc)
target_link_libraries(test_mutex ${LIBS})
