#include "shero/base/Log.h"
#include "shero/net/Buffer.h"
#include "shero/base/Endian.h"
#include "shero/net/websocket/WSStructure.h"

#include <string.h>

namespace shero {
namespace ws {

std::string WSFrameHead::toString() const {
    std::stringstream ss;
    ss << "[WSFrameHead fin = " << fin
       << " rsv1 = " << rsv1
       << " rsv2 = " << rsv2
       << " rsv3 = " << rsv3
       << " opcode = " << opcode
       << " mask = " << mask
       << " payload = " << payload
       << "]";
    return ss.str();
}

std::string WSFrameMessage::EncodeNotFoundMessage() {
    WSFrameMessage::ptr data(
        new WSFrameMessage(WSFrameHead::CLOSE, "WebSocket Not Found Servlet"));
    return EncodeWSFrameMessage(data, true);
}

std::string WSFrameMessage::EncodeWSFrameMessage(std::vector<WSFrameMessage::ptr> &datas) {
    std::string msg;
    for(size_t i = 0; i < datas.size(); i++) {
        msg += EncodeWSFrameMessage(datas[i], i == (datas.size() - 1));
    }
    return msg;
}

std::string WSFrameMessage::EncodeWSFrameMessage(WSFrameMessage::ptr data, bool fin /*= true*/) {
    Buffer buf;
    WSFrameHead head;
    bzero(&head, sizeof(head));
    head.fin = fin;
    head.opcode = data->getOpcode();
    head.mask = true;
    uint64_t size = data->getData().size();
    if(size < 126) {
        head.payload = size;
    } else if(size < 65536) {
        head.payload = 126;
    } else {
        head.payload = 127;
    }
    buf.writeLen(&head, sizeof(head));

    if(head.payload == 126) {
        uint16_t len = size;
        len = shero::byteswapOnLittleEndian(len);
        buf.writeLen(&len, sizeof(len));
    } else if(head.payload == 127) {
        uint64_t len = shero::byteswapOnLittleEndian(size);
        buf.writeLen(&len, sizeof(len));
    }

    char mask[4];
    uint32_t rand_value = rand();
    memcpy(mask, &rand_value, sizeof(mask));
    std::string& msg = data->getData();
    for(size_t i = 0; i < msg.size(); ++i) {
        msg[i] ^= mask[i % 4];
    }

    buf.writeLen(mask, sizeof(mask));
    buf.writeLen(msg.c_str(), msg.size());
    return buf.retrieveAllAsString();
}

WSFrameMessage::ptr WSFrameMessage::DecodeWSFrameMessage(Buffer *buf) {
    uint32_t opcode = 0;
    std::string data;
    int32_t cur_len = 0;

    int n = 2;
    do {
        WSFrameHead head;
        if(buf->readableBytes() < sizeof(head)) {
            LOG_ERROR << "Decode websocket frame message error";
            break;
        }
        buf->readLen(&head, sizeof(head));

        switch(head.opcode) {
            case WSFrameHead::PING: {
                break;
            }
            case WSFrameHead::PONG: {
                break;
            }
            case WSFrameHead::CONTINUE:
            case WSFrameHead::TEXT_FRAME:
            case WSFrameHead::BIN_FRAME: {
                if(!head.mask) {
                    LOG_WARN << "websocket frame head mask != 1";
                    return nullptr;
                }

                uint64_t length = 0;
                // read payload (if head.payload >= 126)
                if(head.payload == 126) {
                    uint16_t len = 0;
                    if(buf->readableBytes() < sizeof(len)) {
                        LOG_ERROR << "Decode websocket frame message error";
                        return nullptr;
                    }
                    buf->readLen(&len, sizeof(len));
                    length = shero::byteswapOnLittleEndian(len);
                } else if(head.payload == 127) {
                    uint64_t len = 0;
                    if(buf->readableBytes() < sizeof(len)) {
                        LOG_ERROR << "Decode websocket frame message error";
                        return nullptr;
                    }
                    buf->readLen(&len, sizeof(len));
                    length = shero::byteswapOnLittleEndian(len);
                } else {
                    length = head.payload;
                }

                // read mask
                char mask[4] = {0};
                if(buf->readableBytes() < sizeof(mask)) {
                    LOG_ERROR << "Decode websocket frame message error";
                    return nullptr;
                }
                buf->readLen(&mask, sizeof(mask));

                // read data
                data.resize(cur_len + length);
                if(buf->readableBytes() < length) {
                    LOG_ERROR << "Decode websocket frame message error";
                    return nullptr;
                }
                buf->readLen(&data[cur_len], length);

                if(head.mask) {
                    for(int32_t i = 0; i < (int32_t)length; i++) {
                        data[cur_len + i] ^= mask[i % 4];
                    }
                }
                cur_len += length;

                if(!opcode && head.opcode != WSFrameHead::CONTINUE) {
                    opcode = head.opcode;
                }

                LOG_INFO << "[DecodeWSFrameMessage] head = " << head.toString()
                    << " data = " << data;

                if(head.fin) {
                    return WSFrameMessage::ptr(
                        new WSFrameMessage(opcode, std::move(data)));
                }

                break;
            }
            default: {
                LOG_WARN << "invalid opcode = " << head.opcode;
                break;
            }
        }
    } while(n--);
    return nullptr;
}

}   // namespace ws
}   // namespace shero