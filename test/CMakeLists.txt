add_executable(test_mutex_thread ./test_mutex_thread.cc)
target_link_libraries(test_mutex_thread ${LIBS})

add_executable(test_log ./test_log.cc)
target_link_libraries(test_log ${LIBS})

add_executable(test_config ./test_config.cc)
target_link_libraries(test_config ${LIBS})

add_executable(test_timer ./test_timer.cc)
target_link_libraries(test_timer ${LIBS})

add_executable(test_hook_server ./test_hook_server.cc)
target_link_libraries(test_hook_server ${LIBS})

add_executable(test_hook_client ./test_hook_client.cc)
target_link_libraries(test_hook_client ${LIBS})

add_executable(test_buffer ./test_buffer.cc)
target_link_libraries(test_buffer ${LIBS})

add_executable(test_coroutine ./test_coroutine.cc)
target_link_libraries(test_coroutine ${LIBS})

add_executable(test_tcp_client ./test_tcp_client.cc)
target_link_libraries(test_tcp_client ${LIBS})

add_executable(test_tcp_server ./test_tcp_server.cc)
target_link_libraries(test_tcp_server ${LIBS})

add_executable(test_http_server ./test_http_server.cc)
target_link_libraries(test_http_server ${LIBS})

add_executable(test_http_client ./test_http_client.cc)
target_link_libraries(test_http_client ${LIBS})

add_executable(test_ws_server ./test_ws_server.cc)
target_link_libraries(test_ws_server ${LIBS})

add_executable(test_ws_client ./test_ws_client.cc)
target_link_libraries(test_ws_client ${LIBS})