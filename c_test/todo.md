
Timer ---  TcpConnection ---  TcpAcceptor
                |
                | <--- Channel
                |
                v        +---- MainEventLoop --- TcpAcceptor
            EventLoop ---+
                |        +---- SubEventLoop  ---+--- Timer --- haven't coroutine
                |                               |
                |                               +--- Connection --- have coroutine
                v
           EPollPoller


TcpAcceptor ---> accept 接受新连接
Connection ---> 已经建立的连接，从 MainEventLoop 放到 SubEventLoop


协程相关:

call GetCoroutinePool()
          |                    +---> Memory
          v                    |
    USER ---> CoroutinePool ---+
      |                        |
      |                        +---> Coroutine
      |                                ^
      |  call getCoroutineInstance()   |
      +--------------------------------+



TodoList:
    - socket          √
    - inetaddress     √
    - endian          √
    - buffer          √
        - bytearray   √
    - hook            √
    - coroutine       √
        - coroutine pool     √
        - memory             √
    - eventloop thread pool  √
    
    - tcp               √
        - server        √
        - accept        √
        - connection    √
        - client        √
    - http              √
        - codec         √
        - dispatcher    √
        - servlet       √
    - websocket
    - protobuf


all: 6696
- base: 1778
- coroutine: 902
- net: 
    - http: 1573
    - tcp: 1103
    - other: 1340

/*
    server: socket -> bind -> setsockopt -> listen -> accept -> read / write
    client: socket -> bind -> connect -> read / write
*/

http 相关:
            http_common.h
                  |
         +--------+---------+
         |                  |
         v                  v
  http11_parser.h    httpclient_parser.h
         |                  |
         +--------+---------+
                  |
                  v
                http.h  <---> http_status.h
                  |
                  v
            http_parser.h
                  |
       +----------+-----------+
       |                      |
       |                      |
       |-> http_server        |-> ws_server
       |-> http_session       |-> ws_session
       |-> http_servlet       |-> ws_servlet
       +-> http_connection    +-> ws_connection


tcp 相关:
    - tcp_server
    - tcp_acceptor
    - tcp_connection
    - thread_pool
                              
        tcp_server
            |
            | <--- tcp_acceptor
            v
         connfd
            | <--- get iothread EventLoop
            | <--- thread_pool
            v                    Resume(connection coroutine)
        iothread  -------------------------------------------------------+
            |                                                            |
            | <--- new tcp_connection(connfd, iothread)                  |
            v                                                            |
     tcp_connection                                                      |
            |                                                            |
            | <--- set coroutine callback(MainServerLoopCorFunc)         |
            v                                                            |
                                                                         |    
    MainServerLoopCorFunc   <--------------------------------------------+
                                      call MainServerLoopCorFunc


配置文件:
    - CoroutinePool.h: 
        - blockSize
        - poolSize