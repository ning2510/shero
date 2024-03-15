set(HEADERS
    AbstractDispatch.h
    AbstractServlet.h
    Address.h
    Buffer.h
    Channel.h
    EPollPoller.h
    EventLoop.h
    EventLoopThread.h
    EventLoopThreadPool.h
    Poller.h
    Socket.h
    Uri.h
)

install(FILES ${HEADERS} DESTINATION include/shero/net)