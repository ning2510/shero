cmake_minimum_required(VERSION 3.10)
project(shero)

SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(.)

SET(SRC_FILES
    shero/log.cc
    shero/util.cc
    shero/config.cc
    shero/thread.cc
)
add_library(shero SHARED ${SRC_FILES})

find_library(YAMLCPP yaml-cpp REQUIRED)
find_library(TINYXML tinyxml REQUIRED)

SET(LIBS
    shero
    pthread
    ${YAMLCPP}
    ${TINYXML}
)

add_executable(test_mutex_thread test/test_mutex_thread.cc)
target_link_libraries(test_mutex_thread ${LIBS})

add_executable(test_log test/test_log.cc)
target_link_libraries(test_log ${LIBS})

add_executable(test_config test/test_config.cc)
target_link_libraries(test_config ${LIBS})