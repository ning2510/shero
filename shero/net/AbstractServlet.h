#ifndef __SHERO_ABSTRACTSERVLET_H
#define __SHERO_ABSTRACTSERVLET_H

#include <memory>
#include <string>

namespace shero {

class AbstractServlet {
public:
    typedef std::shared_ptr<AbstractServlet> ptr;
    AbstractServlet(const std::string &name) 
        : m_name(name) {}
    virtual ~AbstractServlet() {}

    const std::string &getServletName() const { return m_name; }

private:
    std::string m_name;
};

}   // namespace shero

#endif
