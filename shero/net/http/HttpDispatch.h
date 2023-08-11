#ifndef __SHERO_HTTPDISPATCH_H
#define __SHERO_HTTPDISPATCH_H

#include "shero/base/Mutex.h"
#include "shero/net/http/HttpServlet.h"

#include <vector>
#include <memory>
#include <unordered_map>

namespace shero {
namespace http {

class HttpDispatch {
public:
    typedef std::shared_ptr<HttpDispatch> ptr;
    typedef RWMutex RWMutexType;
    HttpDispatch();
    ~HttpDispatch();

    void handle(HttpRequest::ptr req, HttpResponse::ptr res);
    HttpServlet::ptr getMatchedServlet(const std::string &uri);

    void addServlet(const std::string &uri, HttpServlet::ptr slt);
    void addServlet(const std::string &uri, FunctionServlet::ServletCallback cb);

    void addGlobServlet(const std::string &uri, HttpServlet::ptr slt);
    void addGlobServlet(const std::string &uri, FunctionServlet::ServletCallback cb);

    void delServlet(const std::string &uri);
    void delGlobServlet(const std::string &uri);

    HttpServlet::ptr getServlet(const std::string &uri);
    HttpServlet::ptr getGlobServlet(const std::string &uri);

    HttpServlet::ptr getDefaultServlet() const { return m_default; }
    void setDefaultServlet(HttpServlet::ptr v) { m_default = v; }

private:
    RWMutexType m_mutex;
    HttpServlet::ptr m_default;
    // uri(/shero/xxx) -> servlet;
    std::unordered_map<std::string, HttpServlet::ptr> m_servlets;
    // uri(/shero/*) -> servlets
    std::vector<std::pair<std::string, HttpServlet::ptr>> m_globServlets;
};

}   // namespace http
}   // namespace shero

#endif
