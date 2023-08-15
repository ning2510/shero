
#include "shero/net/http/HttpServlet.h"

namespace shero {
namespace http {

// FunctionServlet
FunctionServlet::FunctionServlet(const ServletCallback &cb)
    : HttpServlet("FunctionServlet"),
      m_cb(std::move(cb)) {
}

FunctionServlet::~FunctionServlet() {
    m_cb = nullptr;
}

void FunctionServlet::handle(HttpRequest::ptr req, HttpResponse::ptr res) {
    m_cb(req, res);
}

// NotFoundServlet
NotFoundServlet::NotFoundServlet()
    : HttpServlet("NotFoundServlet") {
}

NotFoundServlet::~NotFoundServlet() {
}

void NotFoundServlet::handle(HttpRequest::ptr req, HttpResponse::ptr res) {
    static const std::string &RES_BODY = 
        "<html>"
            "<head>"
                "<title>404 Not Found</title>"
            "</head>"
            "<body>"
                "<center><h1>404 Not Found</h1></center>"
                "<hr><center>shero/1.0.0</center>"
            "</body>"
        "</html>";

    res->setStatus(HttpStatus::NOT_FOUND);
    res->setHeader("Server", "shero/1.0.0");
    res->setHeader("Content-Type", "text/html;charset=utf-8");
    res->setBody(RES_BODY);
}

}   // namespace http
}   // namespace shero