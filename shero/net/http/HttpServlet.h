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

    virtual void handle(HttpRequest::ptr req, HttpResponse::ptr res) = 0;
};

class FunctionServlet : public HttpServlet {
public:
    typedef std::function<void(HttpRequest::ptr req, 
                    HttpResponse::ptr res)> ServletCallback;
    FunctionServlet(const ServletCallback &cb);
    ~FunctionServlet();

    virtual void handle(HttpRequest::ptr req, HttpResponse::ptr res) override;

private:
    ServletCallback m_cb;
};

class NotFoundServlet : public HttpServlet {
public:
    NotFoundServlet();
    ~NotFoundServlet();

    virtual void handle(HttpRequest::ptr req, HttpResponse::ptr res) override;
};

}   // namespace http
}   // namespace shero

#endif
