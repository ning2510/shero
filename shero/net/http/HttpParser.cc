#include "shero/net/http/HttpParser.h"

#include <iostream>

namespace shero {
namespace http {

bool HttpParser::parserHttpRequest(Buffer *buf) {
    bool ok = true;
    do {
        if(m_state == ParserLine) {
            const char *crlf = buf->findCRLF();
            if(crlf) {
                ok = parserRequestLine(buf->readIndex(), crlf);
                if(ok) {
                    buf->retrieveUntil(crlf + 2);
                    m_state = ParserHeaders;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else if(m_state == ParserHeaders) {
            const char *crlf = buf->findCRLF();
            if(crlf) {
                ok = parserHeaders(buf->readIndex(), crlf);
                buf->retrieveUntil(crlf + 2);
                if(ok) {
                    return true;
                }
            } else {
                return false;
            }
        } else if(m_state == ParserBody) {
            // TODO
        }
    } while(1);
    return ok;
}

bool HttpParser::parserRequestLine(const char* begin, const char* end) {
    const char *start = begin;
    const char *space = std::find(start, end, ' ');
    if(space == end) {
        return false;
    }

    HttpMethod method = StringToHttpMethod(std::string(start, space));
    if(method == HttpMethod::INVALID_METHOD) {
        return false;
    }

    start = space + 1;
    space = std::find(start, end, ' ');
    if(space == end) {
        return false;
    }

    const char *query = std::find(start, space, '?');
    if(query != space) {
        m_request.setPath(std::string(start, query));
        m_request.setQuery(std::string(query, space));
    } else {
        m_request.setPath(std::string(start, space));
    }
    start = space + 1;
    bool it = (end - start == 8) && std::equal(start, end - 1, "HTTP/1.");
    if(!it) {
        return false;
    }

    if(*(end - 1) == '1') {
        m_request.setVersion(0x11);
    } else if(*(end - 1) == '0') {
        m_request.setVersion(0x10);
    } else {
        return false;
    }

    return true;
}

bool HttpParser::parserHeaders(const char* begin, const char* end) {
    const char *it = std::find(begin, end, ':');
    if(it == end) {
        m_state = ParserFinished;
        return true;
    }
    
    std::string key(begin, it);
    ++it;
    while(it < end && isspace(*it)) {
        ++it;
    }

    std::string value(it, end);
    while(!value.empty() && isspace(value[value.size() - 1])) {
        value.resize(value.size() - 1);
    }

    m_request.setHeader(key, value);
    return false;
}


}   // namespace http
}   // namespace shero