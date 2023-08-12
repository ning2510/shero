#ifndef __SHERO_HTTPCLIENT_H
#define __SHERO_HTTPCLIENT_H

#include "shero/net/Uri.h"
#include "shero/base/Mutex.h"
#include "shero/net/Address.h"
#include "shero/net/EventLoop.h"
#include "shero/net/tcp/TcpClient.h"
#include "shero/net/http/HttpParser.h"

#include <map>
#include <vector>
#include <memory>
#include <sstream>
#include <string.h>
#include <functional>
#include <semaphore.h>

namespace shero {
namespace http {

/**
 *   * Note:
 *       - HttpClient currently does not support keep-alive
*/
class HttpClient {
public:
    typedef std::shared_ptr<HttpClient> ptr;
    typedef std::function<void()> SendRequestCallback;
    typedef Mutex MutexType;
    HttpClient(EventLoop *loop, const Address &serverAddr, 
            const std::string &name, bool retry = false);
    ~HttpClient();

    // GET url
    static HttpResponse::ptr DoGet(EventLoop *loop, const std::string &url,
                const std::string &name, bool retry = false,
                const std::map<std::string, std::string> &headers = {},
                const std::string &body = "");
    // GET uri
    static HttpResponse::ptr DoGet(EventLoop *loop, Uri::ptr uri,
                const std::string &name, bool retry = false,
                const std::map<std::string, std::string> &headers = {},
                const std::string &body = "");

    // POST url
    static HttpResponse::ptr DoPost(EventLoop *loop, const std::string &url,
                const std::string &name, bool retry = false,
                const std::map<std::string, std::string> &headers = {},
                const std::string &body = "");
    // POST uri
    static HttpResponse::ptr DoPost(EventLoop *loop, Uri::ptr uri,
                const std::string &name, bool retry = false,
                const std::map<std::string, std::string> &headers = {},
                const std::string &body = "");
    
    // Request url
    static HttpResponse::ptr DoRequest(HttpMethod method, 
                EventLoop *loop, const std::string &url,
                const std::string &name, bool keepAlive = false,
                const std::map<std::string, std::string> &headers = {},
                const std::string &body = "");
    // Request uri
    static HttpResponse::ptr DoRequest(HttpMethod method, 
                EventLoop *loop, Uri::ptr uri,
                const std::string &name, bool keepAlive = false,
                const std::map<std::string, std::string> &headers = {},
                const std::string &body = "");


    static HttpRequest::ptr HttpConstructRequest(HttpMethod method, Uri::ptr uri,
                const std::map<std::string, std::string> &headers = {}, 
                const std::string &body = "");
    static HttpRequest::ptr HttpConstructRequest(HttpMethod method, const std::string &url,
                const std::map<std::string, std::string> &headers = {}, 
                const std::string &body = "");
    

    static HttpResponse::ptr HttpParserResponse(std::vector<std::string> &resHttp);

private:
    void connect();
    void disconnect();

    void setSendCallback(const SendRequestCallback &cb) { m_cb = cb; }
    bool isConnected() { return m_connect && m_client.isConnected(); }
    EventLoop *getEventLoop() const { return m_loop; }
    sem_t *getSem() { return &m_sem; }
    HttpResponse::ptr getResponse() { return m_res; }

    void onSendRequest(HttpRequest::ptr req);
    void onConnection(const TcpConnectionPtr &conn);
    void onMessage(const TcpConnectionPtr &conn, Buffer *buf);

private:
    bool m_connect;
    MutexType m_mutex;
    EventLoop *m_loop;
    TcpClient m_client;

    std::vector<std::string> m_resHttp;
    HttpResponse::ptr m_res;
    TcpConnectionPtr m_conn;
    SendRequestCallback m_cb;

    sem_t m_sem;
};

}   // namespace http
}   // namespace shero

#endif
