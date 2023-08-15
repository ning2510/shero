#include "shero/net/websocket/WSDispatch.h"

#include <fnmatch.h>

namespace shero {
namespace ws {

WSDispatch::WSDispatch() {
    m_default.reset(new NotFoundWSServlet);
}

WSDispatch::~WSDispatch() {
}

void WSDispatch::handle(const std::string &path, 
    WSFrameMessage::ptr req, TcpConnectionPtr conn) {
    
    WSServlet::ptr it = 
        std::dynamic_pointer_cast<WSServlet>(getMatchedServlet(path));
    it->handle(req, conn);
}

AbstractServlet::ptr WSDispatch::getMatchedServlet(const std::string &uri) {
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

void WSDispatch::addServlet(const std::string &uri, AbstractServlet::ptr slt) {
    RWMutexType::WriteLock lock(m_mutex);
    m_servlets[uri] = slt;
}

void WSDispatch::addServlet(const std::string &uri, FunctionWSServlet::HandleCallback cb) {
    FunctionWSServlet::ptr slt = std::make_shared<FunctionWSServlet>(cb);
    addServlet(uri, slt);
}

void WSDispatch::addGlobServlet(const std::string &uri, AbstractServlet::ptr slt) {
    delGlobServlet(uri);
    RWMutexType::WriteLock lock(m_mutex);
    m_globServlets.push_back(std::make_pair(uri, slt));

}

void WSDispatch::addGlobServlet(const std::string &uri, FunctionWSServlet::HandleCallback cb) {
    FunctionWSServlet::ptr slt = std::make_shared<FunctionWSServlet>(cb);
    addGlobServlet(uri, slt);
}

void WSDispatch::delServlet(const std::string &uri) {
    RWMutexType::WriteLock lock(m_mutex);
    m_servlets.erase(uri);
}

void WSDispatch::delGlobServlet(const std::string &uri) {
    RWMutexType::WriteLock lock(m_mutex);
    for(auto it = m_globServlets.begin(); it != m_globServlets.end(); it++) {
        if(it->first == uri) {
            m_globServlets.erase(it);
            break;
        }
    }
}

AbstractServlet::ptr WSDispatch::getServlet(const std::string &uri) {
    RWMutexType::ReadLock lock(m_mutex);
    auto it = m_servlets.find(uri);
    return it == m_servlets.end() ? nullptr : it->second;
}

AbstractServlet::ptr WSDispatch::getGlobServlet(const std::string &uri) {
    RWMutexType::ReadLock lock(m_mutex);
    for(auto &it : m_globServlets) {
        if(it.first == uri) {
            return it.second;
        }
    }
    return nullptr;
}


}   // namespace ws
}   // namespace shero