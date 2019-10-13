#include "AVLTree.hh"
#include <iostream>

int main() {
    crz::AvlTree<int> t1, t2;
    for (int i = 0; i < 1000; ++i)
        t1.insert(i);
    for (int i = 300; i < 1500; ++i)
        t2.insert(i);
    t1.merge(t2);

    int i = 0;
    bool flag = true;
    for (auto x : t1) {
        if (x != i++) {
            flag = false;
            break;
        }
    }

    std::cout << std::boolalpha;
    std::cout << "merge check ok? " << flag << std::endl;
    std::cout << t1;
}
