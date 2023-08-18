#include "shero/net/http/HttpStatus.h"
#include "shero/net/http/HttpParser.h"
#include "shero/net/http/HttpServer.h"

#include <sstream>

namespace shero {
namespace http {

void defaultHttpCallback(const HttpRequest &req, HttpResponse *res) {
    res->setStatus(HttpStatus::NOT_FOUND);
    res->setReason(HttpStatusToString(HttpStatus::NOT_FOUND));
    res->setClose(true);
}

HttpServer::HttpServer(EventLoop *loop, 
    const Address &listenAddr, const std::string &name) 
    : m_server(loop, listenAddr, name),
      m_httpCallback(defaultHttpCallback),
      m_dispatch(new HttpDispatch()) {

    m_server.setConnectionCallback(std::bind(
        &HttpServer::onConnection, this, std::placeholders::_1));
    m_server.setMessageCallback(std::bind(
        &HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
}

void HttpServer::start() {
    m_server.start();
}

void HttpServer::onConnection(const TcpConnectionPtr &conn) {
  if(conn->isConnected()) {
        conn->setContext(HttpParser());
    }
}

void HttpServer::onMessage(const TcpConnectionPtr &conn, Buffer *buf) {
    HttpParser* parser = conn->getMutableContext();

    if(!parser->parserHttpRequest(buf)) {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if(parser->isFinished()) {
        onRequest(conn, parser->getRequest());
        parser->reset();
    }
}

void HttpServer::onRequest(const TcpConnectionPtr &conn, const HttpRequest &req) {
    const std::string connection = req.getHeader("Connection");
    bool close = (connection == "close") || 
            (req.getVersion() == 0x10 && connection != "Keep-Alive");

    HttpResponse res(req.getVersion(), close);
    m_dispatch->handle(req, &res);

    std::stringstream ss;
    ss << res;
    conn->send(ss.str());

    if (res.isClose()) {
        conn->shutdown();
    }
}

}   // namespace http
}   // namespace shero