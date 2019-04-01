#include <algorithm>
#include <ctime>
#include <functional>
#include <iostream>
#include <iterator>
#include <vector>

template <class T, class Cmp = std::less<T>, unsigned N = 4>
class Heap {
    typedef std::vector<T> vc;
    typedef int Node;
    typedef std::function<bool(const T &, const T &)> compare;
    compare cmp;
    vc data;

  public:
    Heap(compare c = Cmp()) : cmp(c) {}
    template <class Iter>
    Heap(Iter beg, Iter end, compare c = Cmp()) : Heap(c) {
        for (; beg != end; ++beg) data.push_back(*beg);
        build();
    }
    template <class Iter>
    void rebuild(Iter beg, Iter end) {
        clear();
        for (; beg != end; ++beg) data.push_back(*beg);
        build();
    }
    void build() {
        Node beg = parent(data.size() - 1);
        for (; beg >= 0; --beg) { heapify(data, beg); }
    }
    void clear() { data.swap(vc()); }
    inline bool empty() { return data.empty(); }
    inline const T &top() { return data[0]; }
    void push(const T &val) {
        data.push_back(T());
        deckey(data, data.size() - 1, val);
    }
    const T pop() {
        T res = data[0];
        data.front() = data.back();
        data.pop_back();
        heapify(data, 0);
        return res;
    }

  private:
    static inline Node parent(Node n) { return (n - 1) / N; }
    static inline Node child(Node n, unsigned i) { return n * N + i; }
    static void heapify(vc &h, Node pos) {
        Node min = pos;
        Node cur = child(pos, 1);
        for (int i = 0; i < N; ++i, ++cur) {
            if (cur >= h.size()) break;
            if (cmp(h[cur], h[min])) min = cur;
        }
        if (pos == min) return;
        std::swap(h[min], h[pos]);
        heapify(h, min);
    }
    static void deckey(vc &h, Node pos, const T &val) {
        Node cur = pos;
        while (cur > 0 && cmp(val, h[parent(cur)])) {
            h[cur] = h[parent(cur)];
            cur = parent(cur);
        }
        h[cur] = val;
    }
};

int main() {
    auto cmp = [](int a, int b) { return a > b; };
    std::vector<int> v{1, 2, 4, 1, 0, 3, 5, 8, 8, 1, 2, 3, 3, 5, 6};
    Heap<int> h(v.begin(), v.end(), cmp);
    while (!h.empty()) std::cout << h.pop() << " ";
    std::cout << std::endl;
}