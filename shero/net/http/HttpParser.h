#ifndef __SHERO_HTTPPARSER_H
#define __SHERO_HTTPPARSER_H

#include "shero/net/http/Http.h"
#include "shero/net/http/http11_parser.h"
#include "shero/net/http/httpclient_parser.h"

#include <memory>

namespace shero {
namespace http {

enum ErrorCode {
    NONE = 0,
    INVALID_METHOD = 1,
    INVALID_VERSION = 2,
};

class HttpRequestParser {
public:
    typedef std::shared_ptr<HttpRequestParser> ptr;
    HttpRequestParser();

    int32_t isFinished();
    size_t execute(char *data, size_t len);
    int32_t hasError();

    uint64_t getContentLength();
    const http_parser &getParser() const { return m_parser; }
    HttpRequest::ptr getData() const { return m_data; }
    void setError(ErrorCode v) { m_error = v; }

private:
    http_parser m_parser;
    HttpRequest::ptr m_data;
    ErrorCode m_error;
};

class HttpResponseParser {
public:
    typedef std::shared_ptr<HttpResponseParser> ptr;
    HttpResponseParser();

    int32_t isFinished();
    size_t execute(char *data, size_t len, bool chunck);
    int32_t hasError();

    uint64_t getContentLength();
    const httpclient_parser &getParser() const { return m_parser; }
    HttpResponse::ptr getData() const { return m_data; }
    void setError(ErrorCode v) { m_error = v; }

private:
    httpclient_parser m_parser;
    HttpResponse::ptr m_data;
    ErrorCode m_error;
};

}   // namespace http
}   // namespace shero

#endif
