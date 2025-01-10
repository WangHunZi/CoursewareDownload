#include <iostream>
#include <string>
#include <cassert>
#include <map>
#include <sstream>
#include <format>
using namespace std;

std::unordered_map<char,int> priority = {
    {'*', 1},
    {'+', 2},
};

string eval(const string &s) {
    int n = s.length();

    if (n == 1) {
        assert('0' <= s[0] && s[0] <= '9');
        return "Node(name=" + s + ")";
    }

    // 标记每个位置的 “嵌套层数”
    int level[n], cur = 0;

    fill(level, level + n, -1);
    for (int i = 0; i < n; i++) {
        if (s[i] == '(') cur++;
        if (s[i] == ')') cur--;
        level[i] = cur;
    }

    auto build_node = [](auto name, auto children) {
        // In C++20:
        // return std::format(
        //     "Node(name='{}',children=[{}])", name, children
        // );

        stringstream ss;
        ss << "Node(name='" << name << "',children=[" << children << "])";
        return ss.str();
    };

    // 如果最外层是配对的括号
    if (s[0] == '(') {
        auto positive = [](int x) { return x > 0; };
        if (all_of(level, level + n - 1, positive)) {
            return build_node("()", eval(s.substr(1, n - 2)));
        }
    }

    // 否则，找到最后一个运算的符号
    int pos = -1;
    for (int i = n - 1; i >= 0; i--) {
        if (level[i] == 0 && (priority.contains(s[i]))) {
            if (pos == -1 || priority[s[pos]] < priority[s[i]]) {
                pos = i;
            }
        }
    }
    assert(pos >= 0);

    string lhs = eval(s.substr(0, pos));
    string rhs = eval(s.substr(pos + 1, -1));

    if (s[pos] == '+') return build_node("+", lhs + "," + rhs);
    if (s[pos] == '*') return build_node("*", lhs + "," + rhs);
    assert(0);
}

int main() {
    cout << eval("1+2") << endl;
    cout << eval("(1+2)*(3+4*5)") << endl;
    cout << eval("4+(1+2+3)*(3+4*5)") << endl;
    cout << eval("1+1+3*4*5+1+1") << endl;
}
