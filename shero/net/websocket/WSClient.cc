#include "shero/base/Log.h"
#include "shero/base/Util.h"
#include "shero/net/websocket/WSClient.h"

namespace shero {
namespace ws {

void WSClient::WSHandShake() {
    m_key = Encodebase64(RandomString(16));
    std::map<std::string, std::string> headers;
    headers["Upgrade"] = "websocket";
    headers["Connection"] = "Upgrade";
    headers["Sec-WebSocket-Version"] = "13";
    headers["Sec-WebSocket-Key"] = m_key;

    http::HttpRequest::ptr req = 
        http::client::HttpClient::HttpConstructRequest(http::HttpMethod::GET, m_uri, headers);

    setState(HTTP);
    std::stringstream ss;
    ss << *req;
    sendHttp(ss.str());
}

WSClient::WSClient(EventLoop *loop, const std::string &url,
        const std::string &name, bool retry /*= false*/)
        : m_connect(false),
          m_loop(loop),
          m_key(""),
          m_messageCallback(nullptr),
          m_connectionCallback(nullptr),
          m_state(NONE),
          m_uri(Uri::Create(url)),
          m_client(loop, *(m_uri->createAddress()), retry, name),
          m_conn(nullptr),
          m_messages() {

    m_client.setConnectionCallback(
        std::bind(&WSClient::onConnection, this, std::placeholders::_1));
    m_client.setMessageCallback(
        std::bind(&WSClient::onMessage, this, std::placeholders::_1, std::placeholders::_2));
}
        
WSClient::~WSClient() {
    // disconnect();
}

void WSClient::connect() {
    m_client.connect();
}

void WSClient::disconnect() {
    if(m_connect) {
        m_connect = false;
        {
            MutexLockGuard lock(m_mutex);
            m_conn.reset();
        }
        m_client.disconnect();
    }
}

void WSClient::sendHttp(const std::string &msg) {
    {
        MutexLockGuard lock(m_mutex);
        m_conn->send(msg);
    }
}

void WSClient::send() {
    std::vector<WSFrameMessage::ptr> tmp;
    {
        MutexLockGuard lock(m_mutex);
        tmp.swap(m_messages);
    }
    std::string msg = WSFrameMessage::EncodeWSFrameMessage(tmp);
    {
        MutexLockGuard lock(m_mutex);
        m_conn->send(msg);
    }
}

bool WSClient::sendWS(const std::string& msg, uint32_t opcode, bool fin /*= true*/) {
    if(!isConnected()) {
        LOG_WARN << "[WSClient] Not currently connected";
        return false;
    }

    WSFrameMessage::ptr data(new WSFrameMessage(opcode, msg));
    m_messages.push_back(data);
    if(!fin) {
        return true;
    }

    send();

    m_messages.clear();
    return true;
}

void WSClient::onConnection(const TcpConnectionPtr &conn) {
    if(conn->isConnected()) {
        LOG_INFO << "[WSClient] Connection UP : " << conn->getPeerAddr().toIpPort();
        {
            MutexLockGuard lock(m_mutex);
            m_conn = conn;
        }
        setState(TCP);
        WSHandShake();
    } else {
        LOG_INFO << "[WSClient] Connection DOWN : " << conn->getPeerAddr().toIpPort();
        disconnect();
    }
}

void WSClient::onMessage(const TcpConnectionPtr &conn, Buffer *buf) {
    if(m_state == NONE || m_state == TCP) {
        LOG_ERROR << "[WSClient] state error, state = " << m_state;
        return ;
    }

    if(m_state == HTTP) {
        std::string msg = buf->retrieveAllAsString();
        // http parser
        http::HttpResponse::ptr res = 
            http::client::HttpClient::HttpParserResponse(msg);
        if(!res) {
            LOG_ERROR << "[WSClient] parser http response error";
            return ;
        }

        LOG_INFO << "recv response = " << *res;
        if(res->getStatus() != http::HttpStatus::SWITCHING_PROTOCOLS) {
            LOG_ERROR << "[WSClient] http response status isn't SWITCHING_PROTOCOLS";
            return ;
        }

        std::string acceptKey = res->getHeader("Sec-WebSocket-Accept");
        m_key = m_key + "T2a6wZlAwhgQNqruZ2YUyg=";
        m_key = Encodebase64(SHA1sum(m_key));
        if(acceptKey != m_key) {
            LOG_ERROR << "[WSClient] http response Sec-WebSocket-Accept isn't equal to key";
            return ;
        }

        setState(WS);
        m_connect = true;
        LOG_INFO << "[WSClient] current protocol is websocket";
        if(m_connectionCallback) {
            m_connectionCallback();
        }
    } else if(m_state == WS) {
        do {
            WSFrameMessage::ptr req = WSFrameMessage::DecodeWSFrameMessage(buf);
            if(!req) {
                break;
            }

            if(m_messageCallback) {
                m_messageCallback(req);
            }   
        } while(buf->readableBytes() > 0);
    }
}


}   // namespace ws
}   // namespace shero