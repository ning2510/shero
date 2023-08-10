#ifndef __SHERO_CALLBACKS_H
#define __SHERO_CALLBACKS_H

#include <memory>
#include <functional>

namespace shero {

class Buffer;
class EventLoop;
class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;
typedef std::function<void(const TcpConnectionPtr &, Buffer *)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr &)> WriteCompleteCallback;

}   // namespace shero

#endif
