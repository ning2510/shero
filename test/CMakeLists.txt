add_executable(test_mutex_thread ./test_mutex_thread.cc)
target_link_libraries(test_mutex_thread ${LIBS})

add_executable(test_log ./test_log.cc)
target_link_libraries(test_log ${LIBS})

add_executable(test_config ./test_config.cc)
target_link_libraries(test_config ${LIBS})

add_executable(test_timer ./test_timer.cc)
target_link_libraries(test_timer ${LIBS})