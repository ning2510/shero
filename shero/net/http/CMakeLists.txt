set(HEADERS
    HttpCommon.h
    HttpDispatch.h
    HttpParser.h
    HttpServer.h
    HttpServlet.h
    HttpStatus.h
)

install(FILES ${HEADERS} DESTINATION include/shero/net/http)