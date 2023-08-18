#include "shero/base/Log.h"
#include "shero/base/Util.h"
#include "shero/base/Mutex.h"
#include "shero/net/Buffer.h"
#include "shero/net/Address.h"
#include "shero/net/EventLoop.h"
#include "shero/net/tcp/TcpClient.h"
#include "shero/net/EventLoopThread.h"

#include <signal.h>
#include <iostream>

using namespace std;
using namespace shero;
using namespace std::placeholders;

class EchoClient {
public:
    EchoClient(EventLoop *loop, const Address &addr, const std::string &name)
        : m_connect(false),
          m_loop(loop),
          m_client(loop, addr, false, name) {

        m_client.setConnectionCallback(std::bind(&EchoClient::onConnection, this, _1));
        m_client.setMessageCallback(std::bind(&EchoClient::onMessage, this, _1, _2));
    }

    ~EchoClient() {
        disconnect();
    }

    void connect() {
        m_client.connect();
    }

    void disconnect() {
        m_client.disconnect();
    }

    void send(const std::string &msg) {
        m_conn->send(msg);
    }

    bool isConnected() {
        return m_connect && m_client.isConnected();
    }

private:
    void onConnection(const TcpConnectionPtr &conn) {
        if(conn->isConnected()) {
            LOG_INFO << "Connectuion UP : " << conn->getPeerAddr().toIpPort();
            {
                MutexLockGuard lock(m_mutex);
                m_conn = conn;
                m_connect = true;
            }
        } else {
            LOG_INFO << "Connection DOWN : " << conn->getPeerAddr().toIpPort();
            {
                MutexLockGuard lock(m_mutex);
                m_conn.reset();
                m_client.disconnect();
                m_connect = false;
            }
        }
    }

    void onMessage(const TcpConnectionPtr &conn, Buffer *buf) {
        std::string msg = buf->retrieveAllAsString();
        std::cout << "recv msg = " << msg << std::endl;
    }

private:
    bool m_connect;
    EventLoop *m_loop;
    TcpConnectionPtr m_conn;
    TcpClient m_client;
    mutable MutexLock m_mutex;
};

int main() {
    std::cout << "Client tid = " << GetThreadId() << std::endl;

    EventLoopThread loopThread;
    Address server(6666);

    EventLoop *loop = loopThread.startLoop();
    EchoClient client(loop, server, "echoClient");
    client.connect();

    string msg;
    while(1) {
        cin >> msg;
        if(msg == "exit" || !client.isConnected()) {
            cout << "client exit" << endl;
            break;
        }
        client.send(msg);
    }

    client.disconnect();
    return 0;
}