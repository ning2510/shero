#ifndef __SHERO_WSSTRUCTURE_H
#define __SHERO_WSSTRUCTURE_H

#include "shero/net/Buffer.h"

#include <vector>
#include <memory>
#include <stdint.h>

namespace shero {
namespace ws {

/*

Websocket message format:
  __________________ 16 ______________________
/ 1   1    1    1       4       1       7      \                 16
+---+----+----+----+----------+----+------------+------------------------------------+
|fin|rsv1|rsv2|rsv3|  opcode  |mask|  payload   |  [For expansion 16 bits] paylod    |
+---+----+----+----+----------+----+------------+------------------------------------+
|                          [For expansion 32 bits] paylod                            |
+-----------------------------------------------+------------------------------------+
|       [For expansion 16 bits] paylod          |        16 bits  Mask-Key           |
+-----------------------------------------------+------------------------------------+
|              16 bits Mask-Key                 |           payload data             |
+-----------------------------------------------+------------------------------------+
|                                  payload data                                      |
+------------------------------------------------------------------------------------+
|                                  payload data                                      |
+------------------------------------------------------------------------------------+

*/

#pragma pack(1)
struct WSFrameHead {
    enum {
        CONTINUE = 1,   // 数据分片帧
        TEXT_FRAME,     // 文本帧
        BIN_FRAME,      // 二进制帧
        CLOSE,          // 断开连接
        PING,           // PING
        PONG            // PONG
    };

    bool fin: 1;
    bool rsv1: 1;
    bool rsv2: 1;
    bool rsv3: 1;
    uint32_t opcode: 4;
    bool mask: 1;
    uint32_t payload: 7;

    std::string toString() const;
};
#pragma pack()


class WSFrameMessage {
public:
    typedef std::shared_ptr<WSFrameMessage> ptr;
    WSFrameMessage(uint32_t opcode, const std::string &data = "")
        : m_opcode(opcode),
          m_data(data) {
    }
    ~WSFrameMessage() {}

    uint32_t getOpcode() const { return m_opcode; }
    void setOpcode(uint32_t opcode) { m_opcode = opcode; }

    const std::string &getData() const { return m_data; }
    std::string &getData() { return m_data; }
    void setData(const std::string &v) { m_data = v; }

    static std::string EncodeWSFrameMessage(WSFrameMessage::ptr data, bool fin = true);
    static std::string EncodeWSFrameMessage(std::vector<WSFrameMessage::ptr> &datas);
    static std::string EncodeNotFoundMessage();

    static WSFrameMessage::ptr DecodeWSFrameMessage(std::string &msg);
    static WSFrameMessage::ptr DecodeWSFrameMessage(Buffer *buf);

private:
    uint32_t m_opcode;
    std::string m_data;
};

}   // namespace ws
}   // namespace shero

#endif
