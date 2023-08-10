#include "shero/net/http/HttpStatus.h"

#include <string.h>

namespace shero {
namespace http {

HttpMethod StringToHttpMethod(const std::string &s) {
#define XX(code, name, str) \
    if(strcmp(#name, s.c_str()) == 0) { \
        return HttpMethod::name; \
    }
    HTTP_METHOD_MAP(XX);
#undef XX
    return HttpMethod::INVALID_METHOD;
}

HttpMethod CharsToHttpMethod(const char *s) {
#define XX(code, name, str) \
    if(strncmp(#name, s, strlen(#name)) == 0) { \
        return HttpMethod::name; \
    }
    HTTP_METHOD_MAP(XX);
#undef XX
    return HttpMethod::INVALID_METHOD;
}

std::string HttpMethodToString(const HttpMethod &method) {
    switch(method) {
#define XX(code, name, str) \
        case HttpMethod::name: \
            return #name;
        HTTP_METHOD_MAP(XX);
#undef XX
        default:
            return "UNKNOW";
    }
}

std::string HttpStatusToString(const HttpStatus &status) {
    switch(status) {
#define XX(code, name, desc) \
        case HttpStatus::name: \
            return #desc;
        HTTP_STATUS_MAP(XX);
#undef XX
        default:
            return "UNKNOW";
    }
}

}   // namespace http
}   // namespace shero