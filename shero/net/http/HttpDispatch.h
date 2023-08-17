#ifndef __SHERO_HTTPDISPATCH_H
#define __SHERO_HTTPDISPATCH_H

#include "shero/net/http/HttpServlet.h"
#include "shero/net/AbstractDispatch.h"

#include <memory>

namespace shero {
namespace http {

class HttpDispatch : public AbstractDispatch {
public:
    typedef std::shared_ptr<HttpDispatch> ptr;
    HttpDispatch();
    ~HttpDispatch();

    void handle(const HttpRequest& req, HttpResponse* res);
    virtual AbstractServlet::ptr getMatchedServlet(const std::string &uri) override;

    virtual void addServlet(const std::string &uri, AbstractServlet::ptr slt) override;
    void addServlet(const std::string &uri, FunctionServlet::ServletCallback cb);

    virtual void addGlobServlet(const std::string &uri, AbstractServlet::ptr slt) override;
    void addGlobServlet(const std::string &uri, FunctionServlet::ServletCallback cb);

    virtual void delServlet(const std::string &uri) override;
    virtual void delGlobServlet(const std::string &uri) override;

    virtual AbstractServlet::ptr getServlet(const std::string &uri) override;
    virtual AbstractServlet::ptr getGlobServlet(const std::string &uri) override;
};

}   // namespace http
}   // namespace shero

#endif
