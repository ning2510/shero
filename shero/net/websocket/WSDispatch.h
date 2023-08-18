#ifndef __SHERO_WSDISPATCH_H
#define __SHERO_WSDISPATCH_H

#include "shero/base/Mutex.h"
#include "shero/net/AbstractDispatch.h"
#include "shero/net/websocket/WSServlet.h"
#include "shero/net/websocket/WSStructure.h"

#include <memory>

namespace shero {
namespace ws {

class WSDispatch : public AbstractDispatch {
public:
    typedef std::shared_ptr<WSDispatch> ptr;
    WSDispatch();
    ~WSDispatch();

    void handle(const std::string &path, WSFrameMessage::ptr req, TcpConnectionPtr conn);
    virtual AbstractServlet::ptr getMatchedServlet(const std::string &uri) override;

    virtual void addServlet(const std::string &uri, AbstractServlet::ptr slt) override;
    void addServlet(const std::string &uri, FunctionWSServlet::HandleCallback cb);

    virtual void addGlobServlet(const std::string &uri, AbstractServlet::ptr slt) override;
    void addGlobServlet(const std::string &uri, FunctionWSServlet::HandleCallback cb);

    virtual void delServlet(const std::string &uri) override;
    virtual void delGlobServlet(const std::string &uri) override;

    virtual AbstractServlet::ptr getServlet(const std::string &uri) override;
    virtual AbstractServlet::ptr getGlobServlet(const std::string &uri) override;
};

}   // namespace ws
}   // namespace shero

#endif
