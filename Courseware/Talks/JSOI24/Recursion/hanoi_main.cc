#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
using namespace std;

using State = array<vector<int>,3>;
vector<State> hanoi(int n);

const vector<string> TEMPLATE = {
    "           A                    B                    C           ",
    "           A                    B                    C           ",
    "           A                    B                    C           ",
    "           A                    B                    C           ",
    "           A                    B                    C           ",
    "           A                    B                    C           ",
    "  _________|_________  _________|_________  _________|_________  ",
};

string render(const State& state) {
    ostringstream ss;
    vector<string> res = TEMPLATE;

    for (int peg = 0; peg < 3; peg++) {
        char peg_name = "ABC"[peg];

        for (int i = 0; i < state[peg].size(); i++) {
            int disk_size = state[peg][i];
            string &row = res[TEMPLATE.size() - 2 - i];
            int pos = row.find(peg_name);
            for (int j = 1; j <= disk_size * 2; j++) {
                row[pos] = row[pos - j] = row[pos + j] = '0' + disk_size;
            }
        }
    }
    for (auto &st : res) {
        ss << st << endl;
    }
    return ss.str();
}

int main() {
    int n;
    cout << "n = ";
    cin >> n;

    for (auto &s: hanoi(n)) {
        string newline;
        std::getline(cin, newline);

        // 清除屏幕
        cout << "\033[2J\033[H";
        for (char ch : render(s)) {
            if ('A' <= ch && ch <= 'C') {
                // 标记了字母的柱子替换成竖线
                cout << "|";
            } else if ('0' <= ch && ch <= '9') {
                // 标记了编号的盘子彩色打印
                cout << "\033[3" << ch << "m";
                cout << "▆";
                cout << "\033[30m";
            } else {
                // 其他字符原样打印
                cout << ch;
            }
        }
    }
}
