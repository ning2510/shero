#include <iostream>
#include <memory>

using namespace std;

class A {
public:
    typedef std::shared_ptr<A> ptr;
    A(int b = 5) : a(b) {}
    ~A() { cout << "~A" << endl; }

    void print() { cout << "a = " << a << endl; }
    static A *getLocal();

private:
    int a;  
};

static thread_local A::ptr local = nullptr;

A *A::getLocal() {
    if(!local) {
        local = std::make_shared<A>();
    }
    return local.get();
}


int main() {
    A *a = A::getLocal();

    a->print();

    return 0;
}