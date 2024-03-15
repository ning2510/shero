set(HEADERS
    Callbacks.h
    TcpAcceptor.h
    TcpClient.h
    TcpConnection.h
    TcpConnector.h
    TcpServer.h
)

install(FILES ${HEADERS} DESTINATION include/shero/net/tcp)