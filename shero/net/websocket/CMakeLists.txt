set(HEADERS
    WSClient.h
    WSDispatch.h
    WSServer.h
    WSServlet.h
    WSStructure.h
)

install(FILES ${HEADERS} DESTINATION include/shero/net/websocket)