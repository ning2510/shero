#ifndef __SHERO_WSSERVLET_H
#define __SHERO_WSSERVLET_H

#include "shero/net/http/HttpParser.h"
#include "shero/net/AbstractServlet.h"
#include "shero/net/tcp/TcpConnection.h"
#include "shero/net/websocket/WSStructure.h"

#include <memory>
#include <string.h>
#include <functional>

namespace shero {
namespace ws {

class WSServlet : public AbstractServlet {
public:
    typedef std::shared_ptr<WSServlet> ptr;
    WSServlet(const std::string &name)
        : AbstractServlet(name) {}
    virtual ~WSServlet() {}

    virtual void handle(WSFrameMessage::ptr msg, TcpConnectionPtr conn) = 0;

};

class FunctionWSServlet : public WSServlet {
public:
    typedef std::shared_ptr<FunctionWSServlet> ptr;
    typedef std::function<void(WSFrameMessage::ptr msg, 
                    TcpConnectionPtr conn)> HandleCallback;
    
    FunctionWSServlet(const HandleCallback &cb);
    ~FunctionWSServlet();

    virtual void handle(WSFrameMessage::ptr msg, TcpConnectionPtr conn) override;

private:
    HandleCallback m_cb;
};

class NotFoundWSServlet : public WSServlet {
public:
    NotFoundWSServlet();
    ~NotFoundWSServlet();

    virtual void handle(WSFrameMessage::ptr msg, TcpConnectionPtr conn) override;
};

}   // namespace ws
}   // namespace shero

#endif
