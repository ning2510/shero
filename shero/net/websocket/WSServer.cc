#include "shero/base/Log.h"
#include "shero/base/Util.h"
#include "shero/net/websocket/WSServer.h"
#include "shero/net/websocket/WSStructure.h"

#include <strings.h>

namespace shero {
namespace ws {

WSServer::WSServer(EventLoop *loop, 
    const Address &addr, const std::string &name)
    : m_stop(true),
      m_loop(loop),
      m_server(loop, addr, name),
      m_dispatch(new WSDispatch()) {
    
    m_server.setConnectionCallback(
        std::bind(&WSServer::onConnection, this, std::placeholders::_1));
    m_server.setMessageCallback(
        std::bind(&WSServer::onMessage, this, std::placeholders::_1, std::placeholders::_2));
}

WSServer::~WSServer() {
}

void WSServer::start() {
    m_server.start();
}

void WSServer::stop() {
    m_stop = true;
    m_loop->quit();
}

void WSServer::setThreadNums(int32_t nums) {
    m_server.setThreadNums(nums);
}

void WSServer::onConnection(const TcpConnectionPtr &conn) {
    if(conn->isConnected()) {
        LOG_INFO << "[WSServer] Connection UP : " << conn->getPeerAddr().toIpPort();
        TcpConnection *it = conn.get();
        if(m_conns.find(it) != m_conns.end()) {
            LOG_ERROR << "something error, [" << it->getName() << "] exist";
            return ;
        }
        m_conns[it] = std::make_pair(TCP, "/");
        conn->setContext(http::HttpParser());
    } else {
        LOG_INFO << "[WSServer] Connection DOWN : " << conn->getPeerAddr().toIpPort();
        TcpConnection *it = conn.get();
        m_conns.erase(it);
    }
}

void WSServer::onMessage(const TcpConnectionPtr &conn, Buffer *buf) {
    TcpConnection *it = conn.get();
    if(m_conns.find(it) == m_conns.end()) {
        LOG_ERROR << "something error, [" << it->getName() << "] not exist";
        return ;
    }

    if(m_conns[it].first == TCP) {
        http::HttpParser* parser = conn->getMutableContext();
        if(!parser->parserHttpRequest(buf)) {
            conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
            conn->shutdown();
        }

        if(parser->isFinished()) {
            HttpHandShake(conn, parser->getRequest());
            parser->reset();
        }
        
    } else if(m_conns[it].first == WEBSOCKET) {
        WSCommunication(conn, buf);
    } else {
        LOG_ERROR << "[" << it->getName() 
            << "] state error, state = " << m_conns[it].first;
    }

}

void WSServer::HttpHandShake(const TcpConnectionPtr &conn, const http::HttpRequest &req) {
    TcpConnection *it = conn.get();
    if(strcasecmp(req.getHeader("Upgrade").c_str(), "websocket")) {
        LOG_ERROR << "[WSServer] http request header's Upgrade field isn't websocket";
        return ;
    }

    if(strcasecmp(req.getHeader("Connection").c_str(), "Upgrade")) {
        LOG_ERROR << "[WSServer] http request header's Connection field isn't Upgrade";
        return ;
    }

    std::string key = req.getHeader("Sec-WebSocket-Key");
    if(key.empty()) {
        LOG_ERROR << "[WSServer] http request Sec-WebSocket-Key is nullptr";
        return ;
    }

    std::string str = key + "T2a6wZlAwhgQNqruZ2YUyg=";
    str = shero::Encodebase64(shero::SHA1sum(str));

    http::HttpResponse res(req.getVersion(), true);
    res.setStatus(http::HttpStatus::SWITCHING_PROTOCOLS);
    res.setReason(http::HttpStatusToString(http::HttpStatus::SWITCHING_PROTOCOLS));
    res.setWebSocket(true);
    res.setReason("WebSocket HandShake");
    res.setHeader("Upgrade", "websocket");
    res.setHeader("Connection", "Upgrade");
    res.setHeader("Sec-WebSocket-Accept", str);

    m_conns[it].first = WEBSOCKET;
    m_conns[it].second = req.getPath();
    std::stringstream ss;
    ss << res;

    LOG_INFO << "recv http request : " << req;

    conn->send(ss.str());
}

void WSServer::WSCommunication(const TcpConnectionPtr &conn, Buffer *buf) {
    TcpConnection *it = conn.get();

    do {
        WSFrameMessage::ptr req = WSFrameMessage::DecodeWSFrameMessage(buf);
        if(!req) {
            break;
        }
        
        m_dispatch->handle(m_conns[it].second, req, conn);
    } while(buf->readableBytes() > 0);
}

}   // namespace ws
}   // namespace shero