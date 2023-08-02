#ifndef __SHERO_SINGLETON_H
#define __SHERO_SINGLETON_H

namespace shero {

template<class T>
class Singleton {
public:
    static T *GetInstance() {
        static T v;
        return &v;
    }
};


}   // namespace shero

#endif
