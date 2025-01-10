#include <map>
#include <set>
#include <iostream>
using namespace std;

const int FAIL = -1;
const int MaxN = 20;

map<int,int> memorized;
set<int> pending;
int call_count = 0;

int try_get_f(int n) {
    // 如果 f(n) 已经计算过，直接返回，否则返回失败
    if (!memorized.contains(n)) {
        return FAIL;
    } else {
        return memorized[n];
    }
}

int f_nr(int n) {
    call_count++;

    if (n <= 1) {
        return n;
    }

    int t1 = try_get_f(n - 1);
    int t2 = try_get_f(n - 2);

    if (t1 == FAIL) pending.insert(n - 1);
    if (t2 == FAIL) pending.insert(n - 2);

    if (t1 == FAIL || t2 == FAIL) {
        return FAIL;
    } else {
        return t1 + t2;
    }
}

int f(int n) {
    call_count = 0;

    pending.insert(n);

    while (pending.size() > 0) {
        // 尝试计算待计算的最小 x
        int x = *pending.begin();

        int y = f_nr(x);
        if (y != FAIL) {
            memorized[x] = y;
            pending.erase(x);
        }
    }

    cout << "# calls to f: " << call_count << endl;
    return try_get_f(n);
}

int main() {
    for (int n : { 0, 1, 2, 3, 4, 5, 10 }) {
        int res = f(n);
        cout << "f(" << n << ") = " << res << endl << endl;
    }
}
