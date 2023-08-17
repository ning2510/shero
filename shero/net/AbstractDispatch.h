#ifndef __SHERO_ABSTRACTDISPATCH_H
#define __SHERO_ABSTRACTDISPATCH_H

#include "shero/base/Mutex.h"
#include "shero/net/AbstractServlet.h"

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace shero {

class AbstractDispatch {
public:
    typedef std::shared_ptr<AbstractDispatch> ptr;
    typedef RWMutex RWMutexType;
    AbstractDispatch() {}
    virtual ~AbstractDispatch() {}

    virtual AbstractServlet::ptr getMatchedServlet(const std::string &uri) = 0;

    virtual void addServlet(const std::string &uri, AbstractServlet::ptr slt) = 0;
    virtual void addGlobServlet(const std::string &uri, AbstractServlet::ptr slt) = 0;

    virtual void delServlet(const std::string &uri) = 0;
    virtual void delGlobServlet(const std::string &uri) = 0;

    virtual AbstractServlet::ptr getServlet(const std::string &uri) = 0;
    virtual AbstractServlet::ptr getGlobServlet(const std::string &uri) = 0;

    AbstractServlet::ptr getDefaultServlet() const { return m_default; }
    void setDefaultServlet(AbstractServlet::ptr v) { m_default = v; }

protected:
    RWMutexType m_mutex;
    AbstractServlet::ptr m_default;
    // uri(/shero/xxx) -> servlet;
    std::unordered_map<std::string, AbstractServlet::ptr> m_servlets;
    // uri(/shero/*) -> servlets
    std::vector<std::pair<std::string, AbstractServlet::ptr>> m_globServlets;
};

}   // namespace shero

#endif
