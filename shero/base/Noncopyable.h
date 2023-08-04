#ifndef __SHERO_NONCOPYABLE_H
#define __SHERO_NONCOPYABLE_H

namespace shero {

class Noncopyable {
public:
    Noncopyable() = default;
    ~Noncopyable() = default;
    Noncopyable(const Noncopyable &) = delete;
    Noncopyable &operator=(const Noncopyable &) = delete;
};

}   // namespace shero

#endif
