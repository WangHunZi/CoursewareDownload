#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <cassert>
#include <tuple>
#include <map>
#include <set>
#include <optional>
#include <iterator>
using namespace std;

using State = array<vector<int>,3>;
using Argument = tuple<State,int,int,int>;
using Result = vector<State>;

template <class T>
void append(vector<T>& dest, const vector<T>& src) {
    for (auto& x : src) {
        dest.push_back(x);
    } 
}

// 全局的 “算出数值” 的列表
std::map<Argument,Result> memorized;
std::set<Argument> pending;

State move_disk(const State &s0, int n, int from, int to) {
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

bool we_know(const Argument &arg) {
    if (memorized.contains(arg)) {
        return true;
    } else {
        pending.insert(arg);
        return false;
    }
}

optional<Result> hanoi_nr(State s0, int n, int from, int to) {
    if (n == 1) {
        return { {move_disk(s0, 1, from, to) } };
    }

    int aux = 3 - from - to; // This is a hack

    // s0 -> s1 -> s2 -> s3
    State s1 = move_disk(s0, n - 1, from, aux);
    State s2 = move_disk(s1, 1, from, to);
    State s3 = move_disk(s0, n, from, to);

    // 需要递归计算的 f(x)
    auto x1 = Argument(s0, n - 1, from, aux);
    auto x2 = Argument(s1, 1, from, to);
    auto x3 = Argument(s2, n - 1, aux, to);

    // 如果 f(x) 的值没有计算出来，立即返回
    if (!we_know(x1)) return nullopt;
    if (!we_know(x2)) return nullopt;
    if (!we_know(x3)) return nullopt;

    // 否则，所有 f(x) 都被已经被计算出来了，直接取出即可
    auto&& steps1 = memorized[x1];
    auto&& steps2 = memorized[x2];
    auto&& steps3 = memorized[x3];

    Result res;
    append(res, steps1);
    append(res, steps2);
    append(res, steps3);

    return res;
}

vector<State> hanoi(int n) {
    State s0;
    for (int i = n; i >= 1; i--) {
        s0[0].push_back(i);
    }

    // 将需要计算的 f(x) 放入列表
    auto arg0 = Argument(s0, n, 0, 2);
    pending.insert(arg0);

    while (pending.size() > 0) {
        // 任取一个需要计算的 x
        auto it = pending.begin();
        std::advance(it, (random() % pending.size()));
        Argument arg = *it;

        // 如果计算成功，就把算出来的值记住
        if (auto res = apply(hanoi_nr, arg)) {
            memorized[arg] = *res;
            pending.erase(arg);
        }
    }

    Result res = memorized[arg0];
    res.insert(res.begin(), s0);
    return res;
}
