#ifndef __SHERO_HTTPPARSER_H
#define __SHERO_HTTPPARSER_H

#include "shero/net/Buffer.h"
#include "shero/net/http/HttpCommon.h"

#include <memory>

namespace shero {
namespace http {

class HttpParser {
public:
    typedef std::shared_ptr<HttpParser> ptr;

    enum HttpParserState {
        ParserLine,
        ParserHeaders,
        ParserBody,
        ParserFinished,
    };

    HttpParser() : m_state(ParserLine) {
    }
    ~HttpParser() {}

    bool parserHttpRequest(Buffer *buf);
    bool isFinished() const { return m_state == ParserFinished; }
    HttpRequest &getRequest() { return m_request; }

    void reset() {
        m_state = ParserLine;
        HttpRequest dummy;
        m_request.swap(dummy);
    }

private:
    bool parserRequestLine(const char* begin, const char* end);
    bool parserHeaders(const char* begin, const char* end);

private:
    HttpParserState m_state;
    HttpRequest m_request;
};

}   // namespace http
}   // namespace shero


#endif
