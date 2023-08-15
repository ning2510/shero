#include "shero/net/websocket/WSServlet.h"
#include "shero/net/websocket/WSStructure.h"

namespace shero {
namespace ws {

FunctionWSServlet::FunctionWSServlet(const HandleCallback &cb)
    : WSServlet("FunctionWSServlet"),
      m_cb(std::move(cb)) {
}

FunctionWSServlet::~FunctionWSServlet() {
    m_cb = nullptr;
}

void FunctionWSServlet::handle(
    WSFrameMessage::ptr msg, TcpConnectionPtr conn) {
    if(m_cb) {
        m_cb(msg, conn);
    }
}


NotFoundWSServlet::NotFoundWSServlet()
    : WSServlet("NotFoundWSServlet") {
}

NotFoundWSServlet::~NotFoundWSServlet() {
}


void NotFoundWSServlet::handle(
    WSFrameMessage::ptr msg, TcpConnectionPtr conn) {
    conn->send(WSFrameMessage::EncodeNotFoundMessage());
}

}   // namespace ws
}   // namespace shero