#include "shero/base/Log.h"
#include "shero/net/http/client/HttpClientParser.h"

#include <string.h>

namespace shero {
namespace http {
namespace client {

static uint64_t String2Uint64(std::string str) {
    uint64_t num = 0;
    for(auto &i : str) {
        if(i >= '0' && i <= '9') {
            num = num * 10 + (i - '0');
        } else {
            return 0;
        }
    }
    return num;
}

// HttpResponseParser
void on_response_http_field(void *data,
        const char *field, size_t flen, const char *value, size_t vlen) {
    HttpResponseParser *parser = static_cast<HttpResponseParser *>(data);
    parser->getData()->setHeader(std::string(field, flen), std::string(value, vlen));
}

void on_response_reason_phrase(void *data, const char *at, size_t length) {
    HttpResponseParser *parser = static_cast<HttpResponseParser *>(data);
    parser->getData()->setReason(std::string(at, length));
}

void on_response_status_code(void *data, const char *at, size_t length) {
    HttpResponseParser *parser = static_cast<HttpResponseParser *>(data);
    HttpStatus status = (HttpStatus)(atoi(at));
    parser->getData()->setStatus(status);
}

void on_response_chunk_size(void *data, const char *at, size_t length) {
}

void on_response_http_version(void *data, const char *at, size_t length) {
    HttpResponseParser *parser = static_cast<HttpResponseParser *>(data);
    uint8_t version = 0;
    if(strncmp(at, "HTTP/1.1", length) == 0) {
        version = 0x11;
    } else if(strncmp(at, "HTTP/1.0", length) == 0) {
        version = 0x10;
    } else {
        LOG_WARN << "invalid request http version = "
            << std::string(at, length);
        parser->setError(ErrorCode::INVALID_VERSION);
        return ;
    }
    parser->getData()->setVersion(version);
}

void on_response_header_done(void *data, const char *at, size_t length) {
}

void on_response_last_chunk(void *data, const char *at, size_t length) {
}

HttpResponseParser::HttpResponseParser()
    : m_error(ErrorCode::NONE) {
    m_data.reset(new HttpResponse);
    httpclient_parser_init(&m_parser);
    m_parser.http_field = on_response_http_field;
	m_parser.reason_phrase = on_response_reason_phrase;
	m_parser.status_code = on_response_status_code;
	m_parser.chunk_size = on_response_chunk_size;
	m_parser.http_version = on_response_http_version;
	m_parser.header_done = on_response_header_done;
	m_parser.last_chunk = on_response_last_chunk;
    m_parser.data = this;
}

int32_t HttpResponseParser::isFinished() {
    return httpclient_parser_finish(&m_parser);
}

size_t HttpResponseParser::execute(char *data, size_t len, bool chunck) {
    if(chunck) {
        httpclient_parser_init(&m_parser);
    }
    size_t offset = httpclient_parser_execute(&m_parser, data, len, 0);
    memmove(data, data + offset, len - offset);
    return offset;
}

size_t HttpResponseParser::execute(std::string &data, size_t len, bool chunck) {
    if(chunck) {
        httpclient_parser_init(&m_parser);
    }
    size_t offset = httpclient_parser_execute(&m_parser, (char *)data.c_str(), len, 0);
    data = data.substr(offset);
    return offset;
}

int32_t HttpResponseParser::hasError() {
    return m_error || httpclient_parser_has_error(&m_parser);
}

uint64_t HttpResponseParser::getContentLength() {
    return String2Uint64(m_data->getHeader("Content-Length"));
}

}   // namespace client
}   // namespace http
}   // namespace shero