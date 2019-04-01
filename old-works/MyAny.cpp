#include <iostream>
#include <string>
#include <typeinfo>
#include <vector>
using namespace std;
class MyClass {
  public:
    int n;
    MyClass(int m) : n(m) {
    }
    void f() {
        cout << n << " func." << endl;
    }
};

class ValueGuard {
  public:
    virtual ~ValueGuard() {
    }
    virtual ValueGuard *clone() = 0;
    virtual const type_info &getinfo() = 0;
};
template <class T> class ValueKeeper : public ValueGuard {
    T val;
    const type_info &info;

  public:
    ValueKeeper(const T &t) : val(t), info(typeid(T)) {
    }
    ValueKeeper(const ValueKeeper<T> &rhs) : val(rhs.val), info(rhs.info) {
    }
    virtual ~ValueKeeper() {
    }
    virtual ValueGuard *clone() {
        return new ValueKeeper<T>(val);
    }
    T &getVal() {
        return val;
    }
    virtual const type_info &getinfo() {
        return info;
    }
};

class MyAny {
    ValueGuard *gd;
    template <class T> friend T &MyAny_cast(const MyAny &a);
    template <class T> friend T *MyAny_cast(const MyAny *a);

  public:
    template <class T> MyAny(const T &t) : gd(new ValueKeeper<T>(t)) {
    }
    MyAny(const MyAny &rhs) : gd(rhs.gd->clone()) {
    }
    ~MyAny() {
        if (gd)
            delete gd;
    }
    MyAny &operator=(const MyAny &rhs) {
        if (gd)
            delete gd;
        gd = rhs.gd->clone();
        return *this;
    }
};

template <class T> T &MyAny_cast(const MyAny &a) {
    return static_cast<ValueKeeper<T> *>(a.gd)->getVal();
}
template <class T> T *MyAny_cast(const MyAny *a) {
    if (a->gd->getinfo() != typeid(T))
        return nullptr;
    return &static_cast<ValueKeeper<T> *>(a->gd)->getVal();
}

int main() {
    while (true) {
        int n;
        string s;
        cin >> n >> s;
        if (n == 0)
            break;
        MyAny a = n;
        cout << MyAny_cast<int>(a) << endl;
        a = s;
        cout << MyAny_cast<string>(a) << endl;
        a = MyClass(n + 1);
        MyAny b = n + 2;
        MyAny *p = new MyAny(MyClass(n + 3));
        MyAny c = MyClass(n + 4);
        c = *p;
        b = *p;
        delete p;
        MyAny d = b;
        MyAny_cast<MyClass>(&a)->f();
        MyAny_cast<MyClass>(&b)->f();
        MyAny_cast<MyClass>(&c)->f();
        MyAny_cast<MyClass>(&d)->f();
        c = s + "OK";
        cout << MyAny_cast<string>(c) << endl;
        int *pi = MyAny_cast<int>(&c);
        if (pi == NULL)
            cout << "not a int" << endl;
        else
            cout << "the int is " << *pi << endl;
        string *ps = MyAny_cast<string>(&c);
        if (ps == NULL)
            cout << "not a string" << endl;
        else
            cout << "the string is " << *ps << endl;
    }
}