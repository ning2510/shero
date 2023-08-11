#include "shero/net/http/Http.h"

#include <sstream>
#include <strings.h>

namespace shero {
namespace http {

bool CaseInsensitiveLess::operator()(
        const std::string &l, const std::string &r) const {
    return strcasecmp(l.c_str(), r.c_str()) < 0;
}

// HttpRequest
HttpRequest::HttpRequest(uint8_t version /*= 0x11*/, bool close /*= true*/)
    : m_method(HttpMethod::GET),
      m_version(version),
      m_path("/"),
      m_close(close) {
}

std::string HttpRequest::getHeader(
        const std::string &key, const std::string &def /*= ""*/) const {
    auto it = m_headers.find(key);
    return it == m_headers.end() ? def : it->second;
}

std::string HttpRequest::getParam(
        const std::string &key, const std::string &def /*= ""*/) const {
    auto it = m_params.find(key);
    return it == m_params.end() ? def : it->second;
}

void HttpRequest::setHeader(const std::string &key, const std::string &val) {
    m_headers[key] = val;
}

void HttpRequest::setParam(const std::string &key, const std::string &val) {
    m_params[key] = val;
}

void HttpRequest::delHeader(const std::string &key) {
    m_headers.erase(key);
}

void HttpRequest::delParam(const std::string &key) {
    m_params.erase(key);
}

bool HttpRequest::hasHeader(
        const std::string &key, std::string *val /*= nullptr*/) {
    auto it = m_headers.find(key);
    if(it == m_headers.end()) {
        return false;
    }
    if(val) {
        *val = it->second;
    }
    return true;
}

bool HttpRequest::hasParam(
        const std::string &key, std::string *val /*= nullptr*/) {
    auto it = m_params.find(key);
    if(it == m_params.end()) {
        return false;
    }
    if(val) {
        *val = it->second;
    }
    return true;
}

std::ostream &HttpRequest::dump(std::ostream &os) const {
    /*
        GET / HTTP/1.1\r\n
        Host: www.example.com\r\n
        Connection: keep-alive\r\n
        \r\n
    */
   os << HttpMethodToString(m_method) << " "
      << m_path
      << (m_query.empty() ? "" : "?")
      << m_query
      << (m_fragment.empty() ? "" : "#")
      << m_fragment
      << " HTTP/"
      << ((uint32_t)(m_version >> 4))
      << "."
      << ((uint32_t)(m_version & 0x0F))
      << "\r\n";
    
    os << "connection: " << (m_close ? "close" : "keep-alive") << "\r\n";
    for(auto &i : m_headers) {
        if(strcasecmp(i.first.c_str(), "connection") == 0) {
            continue;
        }
        os << i.first << ": " << i.second << "\r\n";
    }

    if(!m_body.empty()) {
        os << "content-length: " << m_body.size()
           << "\r\n\r\n" << m_body;
    } else {
        os << "\r\n";
    }
    return os;
}

std::string HttpRequest::toString() const {
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

// HttpResponse
HttpResponse::HttpResponse(uint8_t version /*= 0x11*/, bool close /*= true*/)
    : m_status(HttpStatus::OK),
      m_version(version),
      m_close(close) {
}

std::string HttpResponse::getHeader(
        const std::string &key, const std::string &def /*= ""*/) const {
    auto it = m_headers.find(key);
    return it == m_headers.end() ? def : it->second;
}

void HttpResponse::setHeader(const std::string &key, const std::string &val) {
    m_headers[key] = val;
}

void HttpResponse::delHeader(const std::string &key) {
    m_headers.erase(key);
}

bool HttpResponse::hasHeader(const std::string &key, std::string *val /*= nullptr*/) {
    auto it = m_headers.find(key);
    if(it == m_headers.end()) {
        return false;
    }
    if(val) {
        *val = it->second;
    }
    return true;
}

std::ostream &HttpResponse::dump(std::ostream &os) const {
    /*
        HTTP/1.1 200 OK\r\n
        Content-Type: text/plain\r\n
        Content-Length: 19\r\n
        \r\n
        Hello World! 123456
    */
   os << "HTTP/"
      << ((uint32_t)(m_version >> 4))
      << "."
      << ((uint32_t)(m_version & 0x0F))
      << " "
      << (uint32_t)m_status
      << " "
      << (m_reason.empty() ? HttpStatusToString(m_status) : m_reason)
      << "\r\n";

    for(auto &i : m_headers) {
        if(strcasecmp(i.first.c_str(), "connection") == 0) {
            continue;
        }
        os << i.first << ": " << i.second << "\r\n";
    }
    os << "connection: " << (m_close ? "close" : "keep-alive") << "\r\n";

    if(!m_body.empty()) {
        os << "content-length: " << m_body.size() 
           << "\r\n\r\n" << m_body;
    } else {
        os << "\r\n";
    }
    return os;
}

std::string HttpResponse::toString() const {
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

std::ostream &operator<<(std::ostream &os, const HttpRequest &req) {
    return req.dump(os);
}

std::ostream &operator<<(std::ostream &os, const HttpResponse &res) {
    return res.dump(os);
}

}   // namespace http
}   // namespace shero