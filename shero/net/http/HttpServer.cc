#include "shero/base/Log.h"
#include "shero/net/http/HttpServer.h"
#include "shero/net/http/HttpParser.h"

#include <sstream>
#include <iostream>

namespace shero {
namespace http {

HttpServer::HttpServer(EventLoop *loop, const Address &addr, 
        const std::string &name, bool keepAlive /*= false*/)
        : m_keepAlive(keepAlive),
          m_server(loop, addr, name),
          m_dispatch(new HttpDispatch) {

    // m_server.setThreadNums(1);
    m_server.setConnectionCallback(
        std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    m_server.setMessageCallback(
        std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
}

HttpServer::~HttpServer() {
}

void HttpServer::start() {
    m_server.start();
}

void HttpServer::stop() {
    m_loop->quit();
}

void HttpServer::setThreadNums(int32_t nums) {
    m_server.setThreadNums(nums);
}

void HttpServer::onConnection(const TcpConnectionPtr &conn) {
    if(conn->isConnected()) {
        LOG_INFO << "Http Connection UP : " << conn->getPeerAddr().toIpPort();
    } else {
        LOG_INFO << "Http Connection DOWN : " << conn->getPeerAddr().toIpPort();    
    }
}

void HttpServer::onMessage(const TcpConnectionPtr &conn, Buffer *buf) {
    std::string msg = buf->retrieveAllAsString();
    size_t len = msg.size();

    // 1. parser http request
    HttpRequestParser::ptr reqParser(new HttpRequestParser);
    size_t nparser = reqParser->execute((char *)msg.c_str(), len);
    if(reqParser->hasError()) {
        LOG_ERROR << "[HttpServer] Parsing http request message error";
        return ;
    }

    if(!reqParser->isFinished()) {
        LOG_ERROR << "[HttpServer] Parsing not complete, something erorr";
        return ;
    }

    uint64_t contentLen = reqParser->getContentLength();
    if((nparser > len) || (len - nparser != contentLen)) {
        LOG_ERROR << "[HttpServer] request message body length error";
        return ;
    }

    HttpRequest::ptr req = reqParser->getData();
    if(contentLen > 0) {
        std::string body;
        body.resize(contentLen);
        memcpy(&body[0], &msg[0], len - nparser);   
        req->setBody(body);
    }
    std::string keep_alive = reqParser->getData()->getHeader("Connection");
    if(!strcasecmp(keep_alive.c_str(), "keep-alive")) {
        req->setClose(false);
    }
    
    HttpResponse::ptr res(
        new HttpResponse(req->getVersion(), req->isClose() || !m_keepAlive));
    
    // 2. get matched servlet
    m_dispatch->handle(req, res);

    // 3. send http response
    std::stringstream ss;
    ss << *res;
    conn->send(ss.str());
}


}   // namespace http
}   // namespace shero