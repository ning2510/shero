set(HEADERS
    http11_common.h
    HttpClient.h
    httpclient_parser.h
    HttpClientParser.h
)

install(FILES ${HEADERS} DESTINATION include/shero/net/http/client)