// a simple digital-circuit simulator
#include <boost/shared_ptr.hpp>
#include <functional>
#include <iostream>
#include <list>
#include <string>

typedef std::function<void(void)> action_type;

class Wire {
    int signal_value;
    std::list<action_type> procs;

  public:
    Wire() : signal_value(0), procs() {}
    void set_signal(int value) {
        signal_value = value;
        for (auto proc : procs) proc();
    }
    inline int get_signal() { return signal_value; }
    void add_action(action_type action) {
        procs.push_front(action);
        action();
    }
};

void not_gate(Wire &in, Wire &out) {
    action_type action = [&]() -> void {
        int out_value = !in.get_signal();
        out.set_signal(out_value);
    };
    in.add_action(action);
}

void and_gate(Wire &in1, Wire &in2, Wire &out) {
    action_type action = [&]() -> void {
        int out_value = in1.get_signal() && in2.get_signal();
        out.set_signal(out_value);
    };
    in1.add_action(action);
    in2.add_action(action);
}

void or_gate(Wire &in1, Wire &in2, Wire &out) {
    action_type action = [&]() -> void {
        int out_value = in1.get_signal() || in2.get_signal();
        out.set_signal(out_value);
    };
    in1.add_action(action);
    in2.add_action(action);
}

void xor_gate(Wire &in1, Wire &in2, Wire &out) {
    action_type action = [&]() -> void {
        int out_value = in1.get_signal() ^ in2.get_signal();
        out.set_signal(out_value);
    };
    in1.add_action(action);
    in2.add_action(action);
}

void probe(Wire &s, const std::string &name) {
    auto temp = [&name, &s]() -> void {
        std::cout << name << " value: " << s.get_signal() << std::endl;
    };
    s.add_action(temp);
}

int main() {
    Wire a, b, c, d;
    // probe(a, "wire-a");
    // probe(b, "wire-b");
    // probe(c, "wire-c");
    probe(d, "wire-d");
    and_gate(a, b, c);
    not_gate(c, d);
    a.set_signal(1);
    b.set_signal(1);
    b.set_signal(0);
}