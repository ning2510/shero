#ifndef __SHERO_HTTP_H
#define __SHERO_HTTP_H

#include "shero/net/http/HttpStatus.h"

#include <map>
#include <memory>

namespace shero {
namespace http {

struct CaseInsensitiveLess {
    bool operator()(const std::string &l, const std::string &r) const;
};


class HttpRequest {
public:
    typedef std::shared_ptr<HttpRequest> ptr;
    typedef std::map<std::string, std::string, CaseInsensitiveLess> MapType;
    HttpRequest(uint8_t version = 0x11, bool close = true);

    HttpMethod getMethod() const { return m_method; }
    void setMethod(HttpMethod method) { m_method = method; }

    uint8_t getVersion() const { return m_version; }
    void setVersion(uint8_t version) { m_version = version; }
    
    const std::string &getPath() const { return m_path; }
    void setPath(const std::string &path) { m_path = path; }
    
    const std::string &getQuery() const { return m_query; }
    void setQuery(const std::string &query) { m_query = query; }
    
    const std::string &getFragment() const { return m_fragment; }
    void setFragment(const std::string &fragment) { m_fragment = fragment; }

    const std::string &getBody() const { return m_body; }
    void setBody(const std::string &body) { m_body = body; }

    bool isClose() const { return m_close; }
    void setClose(bool close) { m_close = close; }

    bool isWebSocket() const { return m_websocket; }
    void setWebSocket(bool v) { m_websocket = v; }

    const MapType &getHeaders() const { return m_headers; }
    void setHeaders(const MapType& v) { m_headers = v; }

    const MapType &getParams() const { return m_params; }
    void setParams(const MapType& v) { m_params = v; }

    std::string getHeader(const std::string &key, const std::string &def = "") const;
    std::string getParam(const std::string &key, const std::string &def = "") const;

    void setHeader(const std::string &key, const std::string &val);
    void setParam(const std::string &key, const std::string &val);

    void delHeader(const std::string &key);
    void delParam(const std::string &key);

    bool hasHeader(const std::string &key, std::string *val = nullptr);
    bool hasParam(const std::string &key, std::string *val = nullptr);
    
    std::ostream &dump(std::ostream &os) const;
    std::string toString() const;

private:
    HttpMethod m_method;
    uint8_t m_version;
    std::string m_path;
    std::string m_query;
    std::string m_fragment;
    std::string m_body;
    bool m_close;   // false: keep-alive    true: close
    bool m_websocket;

    MapType m_headers;
    MapType m_params;
};

class HttpResponse {
public:
    typedef std::shared_ptr<HttpResponse> ptr;
    typedef std::map<std::string, std::string, CaseInsensitiveLess> MapType;
    HttpResponse(uint8_t version = 0x11, bool close = true);

    HttpStatus getStatus() const { return m_status; }
    void setStatus(HttpStatus status) { m_status = status; }

    uint8_t getVersion() const { return m_version; }
    void setVersion(uint8_t version) { m_version = version; }

    const std::string &getBody() const { return m_body; }
    void setBody(const std::string &body) { m_body = body; }

    const std::string &getReason() const { return m_reason; }
    void setReason(const std::string &reason) { m_reason = reason; }

    bool isClose() const { return m_close; }
    void setClose(bool close) { m_close = close; }

    bool isWebSocket() const { return m_websocket; }
    void setWebSocket(bool v) { m_websocket = v; }

    const MapType &getHeaders() const { return m_headers; }
    void setHeaders(const MapType& v) { m_headers = v; }

    std::string getHeader(const std::string &key, const std::string &def = "") const;
    void setHeader(const std::string &key, const std::string &val);
    void delHeader(const std::string &key);
    bool hasHeader(const std::string &key, std::string *val = nullptr);

    std::ostream &dump(std::ostream &os) const;
    std::string toString() const;

private:
    HttpStatus m_status;
    uint8_t m_version;
    std::string m_body;
    std::string m_reason;
    bool m_close;
    bool m_websocket;
    
    MapType m_headers;
};

std::ostream &operator<<(std::ostream &os, const HttpRequest &req);
std::ostream &operator<<(std::ostream &os, const HttpResponse &res);

}   // namespace http
}   // namespace shero

#endif
