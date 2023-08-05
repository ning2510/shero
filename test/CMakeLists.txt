add_executable(test_mutex_thread ./test_mutex_thread.cc)
target_link_libraries(test_mutex_thread ${LIBS})

add_executable(test_log ./test_log.cc)
target_link_libraries(test_log ${LIBS})

add_executable(test_config ./test_config.cc)
target_link_libraries(test_config ${LIBS})

add_executable(test_timer ./test_timer.cc)
target_link_libraries(test_timer ${LIBS})

add_executable(test_socket_server ./test_socket_server.cc)
target_link_libraries(test_socket_server ${LIBS})

add_executable(test_socket_client ./test_socket_client.cc)
target_link_libraries(test_socket_client ${LIBS})