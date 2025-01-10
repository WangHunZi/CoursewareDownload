#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <cassert>
using namespace std;

template <class T>
void append(vector<T>& dest, const vector<T>& src) {
    for (auto& x : src) {
        dest.push_back(x);
    } 
    // dest.insert(dest.end(), src.begin(), src.end());
}

using State = array<vector<int>,3>;
using Result = vector<State>;

State move_disk(const State& s0, int n, int from, int to) {
    State s = s0;
    int disks[n];

    for (int i = 0; i < n; i++) {
        disks[i] = s[from].back();
        s[from].pop_back();
    }

    for (int i = n - 1; i >= 0; i--) {
        s[to].push_back(disks[i]);
    }

    return s;
}

Result hanoi_r(State s0, int n, int from, int to) {
    if (n == 1) {
        return { move_disk(s0, 1, from, to) };
    }

    int aux = 3 - from - to; // This is a hack

    // s0 -> s1 -> s2 -> s3
    State s1 = move_disk(s0, n - 1, from, aux);
    State s2 = move_disk(s1, 1, from, to);
    State s3 = move_disk(s0, n, from, to);

    // 可以任意交换调用顺序！
    auto&& steps2 = hanoi_r(s1, 1, from, to);
    auto&& steps3 = hanoi_r(s2, n - 1, aux,  to);
    auto&& steps1 = hanoi_r(s0, n - 1, from, aux);

    hanoi_r(s2, n - 1, aux,  to);

    // 检查返回的状态序列是否正确
    assert(steps1.back() == s1);
    assert(steps2.back() == s2);
    assert(steps3.back() == s3);

    // 将状态拼接起来返回
    Result res;
    append(res, steps1);
    append(res, steps2);
    append(res, steps3);

    return res;
}

Result hanoi(int n) {
    State s0;
    for (int i = n; i >= 1; i--) {
        s0[0].push_back(i);
    }

    auto&& res = hanoi_r(s0, n, 0, 2);
    res.insert(res.begin(), s0);
    return res;
}
