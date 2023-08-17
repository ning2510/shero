#include <memory>
#include <iostream>
#include <functional>

using namespace std;

class B {
public:
    typedef std::shared_ptr<B> ptr;
    B(std::function<void()> c) : cb(c) {}    
    void print() {
        cout << "B print start" << endl;
        cb();
        cout << "B print end" << endl;
    }

    std::function<void()> cb;
};

class A {
public:
    A() {
        b = std::make_shared<B>(std::bind(&A::print, this));
    }

    static void *print(void *arg) {
        std::cout << "111\n";
    }

    B::ptr b;
};

void *test() {
    cout << "test \n";
    return nullptr;
}

int main() {
    // A a;
    // a.b->print();

    function<void()> cb = test;
    cb();

    return 0;
}