#include "shero/base/Log.h"
#include "shero/base/Util.h"
#include "shero/net/http/HttpClient.h"

#include <string.h>
#include <strings.h>

namespace shero {
namespace http {

// GET url
HttpResponse::ptr HttpClient::DoGet(EventLoop *loop, const std::string &url,
                const std::string &name, bool retry /*= false*/,
                const std::map<std::string, std::string> &headers /*= {}*/,
                const std::string &body /*= ""*/) {
    Uri::ptr uri = Uri::Create(url);
    return DoRequest(HttpMethod::GET, loop, uri, name, retry, headers, body);
}

// GET uri
HttpResponse::ptr HttpClient::DoGet(EventLoop *loop, Uri::ptr uri,
                const std::string &name, bool retry /*= false*/,
                const std::map<std::string, std::string> &headers /*= {}*/,
                const std::string &body /*= ""*/) {
    return DoRequest(HttpMethod::GET, loop, uri, name, retry, headers, body);
}

// POST url
HttpResponse::ptr HttpClient::DoPost(EventLoop *loop, const std::string &url,
                const std::string &name, bool retry /*= false*/,
                const std::map<std::string, std::string> &headers /*= {}*/,
                const std::string &body /*= ""*/) {
    Uri::ptr uri = Uri::Create(url);
    return DoRequest(HttpMethod::POST, loop, uri, name, retry, headers, body);
}

// POST uri
HttpResponse::ptr HttpClient::DoPost(EventLoop *loop, Uri::ptr uri,
                const std::string &name, bool retry /*= false*/,
                const std::map<std::string, std::string> &headers /*= {}*/,
                const std::string &body /*= ""*/) {
    return DoRequest(HttpMethod::POST, loop, uri, name, retry, headers, body);
}

// Request url
HttpResponse::ptr HttpClient::DoRequest(HttpMethod method, 
                EventLoop *loop, const std::string &url,
                const std::string &name, bool retry /*= false*/,
                const std::map<std::string, std::string> &headers /*= {}*/,
                const std::string &body /*= ""*/) {
    Uri::ptr uri = Uri::Create(url);
    return DoRequest(method, loop, uri, name, retry, headers, body);
}

// Request uri
HttpResponse::ptr HttpClient::DoRequest(HttpMethod method, 
                EventLoop *loop, Uri::ptr uri,
                const std::string &name, bool retry /*= false*/,
                const std::map<std::string, std::string> &headers /*= {}*/,
                const std::string &body /*= ""*/) {
    
    // 1. construct HttpRequest
    HttpRequest::ptr req(new HttpRequest);
    req->setMethod(method);
    req->setPath(uri->getPath());
    req->setQuery(uri->getQuery());
    req->setFragment(uri->getFragment());

    bool host = false;
    for(auto &i : headers) {
        if(strcasecmp(i.first.c_str(), "connection") == 0) {
            if(strcasecmp(i.second.c_str(), "keep-alive") == 0) {
                req->setClose(false);
            }
            continue;
        }

        if(!host && strcasecmp(i.first.c_str(), "host") == 0) {
            host = !i.second.empty();
        }
        req->setHeader(i.first, i.second);
    }

    if(!host) {
        req->setHeader("Host", uri->getHost());
    }

    LOG_INFO << "port = " << uri->getPort() << ", host = " << uri->getHost();

    // 2. construct HttpClient instance
    Address::ptr addr = uri->createAddress();
    HttpClient::ptr client(new HttpClient(loop, *addr, name, retry));

    // 3. set callback
    // client->setSendCallback(std::bind(&HttpClient::onSendRequest, client.get(), req));
    std::weak_ptr<HttpClient> client_weak = client;
    client->setSendCallback([client_weak, req]() {
        auto client = client_weak.lock();
        if(client) {
            client->onSendRequest(req);
        }
    });

    client->connect();

    sem_wait(client->getSem());
    HttpResponse::ptr res = client->getResponse();
    std::cout << "use count = " << client.use_count() << std::endl;
    return res;
}

HttpClient::HttpClient(EventLoop *loop, const Address &serverAddr, 
        const std::string &name, bool retry /*= false*/)
        : m_connect(false),
          m_loop(loop),
          m_client(loop, serverAddr, retry, name),
          m_resHttp(),
          m_res(nullptr),
          m_cb(nullptr) {
    
    sem_init(&m_sem, 0, 0);
    m_client.setConnectionCallback(
        std::bind(&HttpClient::onConnection, this, std::placeholders::_1));
    m_client.setMessageCallback(
        std::bind(&HttpClient::onMessage, this, std::placeholders::_1, std::placeholders::_2));
}

HttpClient::~HttpClient() {
    sem_destroy(&m_sem);
}

void HttpClient::connect() { 
    m_client.connect(); 
}

void HttpClient::disconnect() {
    m_conn.reset();
    m_client.disconnect(); 
}

void HttpClient::onSendRequest(HttpRequest::ptr req) {
    std::stringstream ss;
    ss << *req;
    std::string msg = ss.str();

    LOG_INFO << "send len = " << msg.size() 
        << "\n" << msg;

    m_conn->send(msg);

    /**
     *   如果在 bind 时传递的 client 是 shared_ptr,
     * bind 会增加引用计数, 清空 m_cb 以减少引用计数
    */
    m_cb = nullptr;
}

// Connect Callback
void HttpClient::onConnection(const TcpConnectionPtr &conn) {
    if(conn->isConnected()) {
        LOG_INFO << "[HttpClient] Connectuion UP : " << conn->getPeerAddr().toIpPort();
        {
            MutexType::Lock lock(m_mutex);
            m_conn = conn;
            m_connect = true;
            if(m_cb) {
                m_cb();
            }
        }
    } else {
        LOG_INFO << "[HttpClient] Connectuion DOWN : " << conn->getPeerAddr().toIpPort();
        {
            MutexType::Lock lock(m_mutex);
            m_conn.reset();
            m_client.disconnect();
            m_connect = false;
            onMessage(nullptr, nullptr);
        }
    }
}

// Message Callback
void HttpClient::onMessage(const TcpConnectionPtr &conn, Buffer *buf) {
    if(m_connect) {
        m_resHttp.push_back(buf->retrieveAllAsString());
        return ;
    }

    if(m_resHttp.empty()) {
        LOG_ERROR << "recv http response message is nullptr";
        return ;
    }

    // Before parser
    // for(size_t i = 0; i < m_resHttp.size(); i++) {
    //     LOG_INFO << "i = " << i << "\n" << m_resHttp[i];
    // }

    std::string msg = m_resHttp[0];
    size_t len = msg.size();

    // 1. parser http response
    HttpResponseParser::ptr resParser(new HttpResponseParser);
    resParser->execute(msg, len, false);

    // size_t nparser = resParser->execute(msg, len, false);
    // std::cout << "len = " << len << " len2 = " << msg.size() << " nparser = " << nparser << '\n';
    if(resParser->hasError()) {
        LOG_ERROR << "[HttpClient] Parsing http response message error";
        return ;
    }

    if(!resParser->isFinished()) {
        LOG_ERROR << "[HttpClient] Parsing not complete, something erorr";
        return ;
    }

    HttpResponse::ptr res = resParser->getData();
    auto *clientParser = resParser->getParser();
    if(clientParser->chunked) {
        // 2. parser http body
        std::string body;
        len = msg.size();
        size_t index = 1;
        do {
            do {
                if(index >= m_resHttp.size() && msg.empty()) {
                    LOG_ERROR << "[HttpClient] Parsing error, the message received is incomplete";
                    return ;
                }

                if(index < m_resHttp.size()) {
                    msg += m_resHttp[index++];
                }

                // If there is \r\n in the beginning part, it should be removed
                if(msg.size() >= 2 && msg[0] == '\r' && msg[1] == '\n') {
                    msg = msg.substr(2);
                }

                len = msg.size();
                msg[len] = '\0';
                size_t np = resParser->execute(msg, len, true); 
                if(resParser->hasError()) {
                    LOG_ERROR << "[HttpClient] Parsing http response message error";
                    return ;
                }

                len -= np;
            } while(!resParser->isFinished());

            // std::cout << "[HttpClient] Response http message content_len = " 
            //     << clientParser->content_len << " len = " << len << '\n';

            if(clientParser->content_len <= (int32_t)len) {
                body += msg.substr(0, clientParser->content_len);
                msg = msg.substr(clientParser->content_len);
                len -= clientParser->content_len;
            } else {
                body += msg;
                size_t remain = clientParser->content_len - len;
                while(remain > 0) {
                    if(index >= m_resHttp.size()) {
                        LOG_ERROR << "[HttpClient] Parsing error, the message received is incomplete, index = " << index;
                        return ;
                    }
                    msg = m_resHttp[index++];
                    len = msg.size();
                    if(remain >= len) {
                        body += msg;
                        remain -= len;
                        len = 0;
                        msg.clear();
                    } else {
                        body += msg.substr(0, remain);
                        msg = msg.substr(remain, len - remain);
                        len = msg.size();
                        remain = 0;
                    }

                }
            }
        } while(!clientParser->chunks_done);

        res->setBody(body);
    } else {
        for(size_t i = 1; i < m_resHttp.size(); i++) {
            msg += m_resHttp[i];
        }
        len = msg.size();

        // 2. parser http body
        uint64_t contentLen = resParser->getContentLength();
        LOG_INFO << "content length = " << contentLen
            << " actual length = " << len;
        if(len < contentLen) {
            LOG_ERROR << "[HttpClient] response message body length error";
            return ;
        }
    
        std::string body;
        body.resize(contentLen);
        memcpy(&body[0], &msg[0], contentLen);
        res->setBody(body);
    }

    LOG_INFO << "After parser:\n" << res->getBody();
    m_res = res;
    sem_post(&m_sem);
    disconnect();
}


}   // namespace http
}   // namespace shero