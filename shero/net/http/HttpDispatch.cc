#include "shero/net/http/HttpDispatch.h"

#include <fnmatch.h>

namespace shero {
namespace http {

HttpDispatch::HttpDispatch()
    : AbstractDispatch() {
    m_default.reset(new NotFoundServlet);
}

HttpDispatch::~HttpDispatch() {
}

void HttpDispatch::handle(const HttpRequest& req, HttpResponse* res) {
    HttpServlet::ptr it = 
        std::dynamic_pointer_cast<HttpServlet>(getMatchedServlet(req.getPath()));
    it->handle(req, res);
}

AbstractServlet::ptr HttpDispatch::getMatchedServlet(const std::string &uri) {
    RWMutexType::ReadLock rlock(m_mutex);
    auto it = m_servlets.find(uri);
    if(it != m_servlets.end()) {
        return it->second;
    }

    for(auto &it : m_globServlets) {
        if(fnmatch(it.first.c_str(), uri.c_str(), 0) == 0) {
            return it.second;
        }
    }

    return m_default;
}

void HttpDispatch::addServlet(const std::string &uri, AbstractServlet::ptr slt) {
    RWMutexType::WriteLock lock(m_mutex);
    m_servlets[uri] = slt;
}

void HttpDispatch::addServlet(const std::string &uri, FunctionServlet::ServletCallback cb) {
    FunctionServlet::ptr slt = std::make_shared<FunctionServlet>(cb);
    addServlet(uri, slt);
}

void HttpDispatch::addGlobServlet(const std::string &uri, AbstractServlet::ptr slt) {
    delGlobServlet(uri);
    RWMutexType::WriteLock lock(m_mutex);
    m_globServlets.push_back(std::make_pair(uri, slt));
}

void HttpDispatch::addGlobServlet(const std::string &uri, FunctionServlet::ServletCallback cb) {
    FunctionServlet::ptr slt = std::make_shared<FunctionServlet>(cb);
    addGlobServlet(uri, slt);
}

void HttpDispatch::delServlet(const std::string &uri) {
    RWMutexType::WriteLock lock(m_mutex);
    m_servlets.erase(uri);
}

void HttpDispatch::delGlobServlet(const std::string &uri) {
    RWMutexType::WriteLock lock(m_mutex);
    for(auto it = m_globServlets.begin(); it != m_globServlets.end(); it++) {
        if(it->first == uri) {
            m_globServlets.erase(it);
            break;
        }
    }
}

AbstractServlet::ptr HttpDispatch::getServlet(const std::string &uri) {
    RWMutexType::ReadLock lock(m_mutex);
    auto it = m_servlets.find(uri);
    return it == m_servlets.end() ? nullptr : it->second;
}

AbstractServlet::ptr HttpDispatch::getGlobServlet(const std::string &uri) {
    RWMutexType::ReadLock lock(m_mutex);
    for(auto &it : m_globServlets) {
        if(it.first == uri) {
            return it.second;
        }
    }
    return nullptr;
}


}   // namespace http
}   // namespace shero
