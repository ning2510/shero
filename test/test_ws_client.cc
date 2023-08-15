#include "shero/base/Log.h"
#include "shero/base/Util.h"
#include "shero/net/Address.h"
#include "shero/net/http/Http.h"
#include "shero/net/EventLoopThread.h"
#include "shero/net/http/HttpClient.h"
#include "shero/net/websocket/WSClient.h"
#include "shero/net/websocket/WSStructure.h"

#include <memory>
#include <signal.h>
#include <unistd.h>
#include <assert.h>
#include <semaphore.h>

using namespace std;
using namespace shero;
using namespace ws;

sem_t sem;

void Quit(int sig) {
    std::cout << "222\n";
    sem_post(&sem);
}

class WSClientTest {
public:
    typedef std::shared_ptr<WSClientTest> ptr;
    WSClientTest(EventLoop *loop, const std::string &url,
        const std::string &name, bool retry = false)
        : m_client(loop, url, name, retry) {
        
        m_client.setWSConnectionCallback(
            std::bind(&WSClientTest::OnWSConnection, this));
        m_client.setWSMessageCallback(
            std::bind(&WSClientTest::onWSMessage, this, std::placeholders::_1));
    }
    ~WSClientTest() {
        disconnect();
    }

    bool send(const std::string& msg, uint32_t opcode, bool fin = true) {
        return m_client.sendWS(msg, opcode, fin);
    }

    void connect() {
        m_client.connect();
    }

    void disconnect() {
        m_client.disconnect();
    }

    bool isConnected() {
        return m_client.isConnected();
    }

private:
    void OnWSConnection() {
        bool rt = send("hello", WSFrameHead::TEXT_FRAME, false);
        assert(rt);
        rt = send(" world", WSFrameHead::TEXT_FRAME, true);
        assert(rt);
        rt = send("hello, this is shero", WSFrameHead::TEXT_FRAME, true);
        assert(rt);
        
        // sem_post(&sem);
    }

    void onWSMessage(const WSFrameMessage::ptr &msg) {
        cout << "onWSMessage: " << msg->getData() << endl;
    }

private:
    WSClient m_client;
};

int main() {
    sem_init(&sem, 0, 0);
    signal(SIGINT, Quit);

    cout << "Client tid = " << GetThreadId() << endl;
    EventLoopThread loopThread;
    EventLoop *loop = loopThread.startLoop();

    WSClientTest client(loop, "http://127.0.0.1:9999/shero/xxx", "WSClientTest");
    client.connect();

    std::string msg;
    cin >> msg;
    client.send(msg, WSFrameHead::TEXT_FRAME, true);

    sem_wait(&sem);
    sem_destroy(&sem);
    return 0;
}