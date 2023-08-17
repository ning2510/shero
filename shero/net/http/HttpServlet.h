#ifndef __SHERO_HTTPSERVLET_H
#define __SHERO_HTTPSERVLET_H

#include "shero/net/AbstractServlet.h"
#include "shero/net/http/HttpParser.h"

#include <memory>
#include <string.h>
#include <functional>

namespace shero {
namespace http {

class HttpServlet : public AbstractServlet {
public:
    typedef std::shared_ptr<HttpServlet> ptr;
    HttpServlet(const std::string &name)
        : AbstractServlet(name) {}
    virtual ~HttpServlet() {}

    virtual void handle(const HttpRequest& req, HttpResponse* res) = 0;
};

class FunctionServlet : public HttpServlet {
public:
    typedef std::function<void(const HttpRequest& req, 
                        HttpResponse* res)> ServletCallback;
    FunctionServlet(const ServletCallback &cb);
    ~FunctionServlet();

    virtual void handle(const HttpRequest& req, HttpResponse* res) override;

private:
    ServletCallback m_cb;
};

class NotFoundServlet : public HttpServlet {
public:
    NotFoundServlet();
    ~NotFoundServlet();

    virtual void handle(const HttpRequest& req, HttpResponse* res) override;
};

}   // namespace http
}   // namespace shero

#endif
