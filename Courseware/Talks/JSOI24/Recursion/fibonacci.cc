#include <iostream>
#include <vector>
#include <string>
using namespace std;

string f(int n) {
    if (n == 0) return "a";
    if (n == 1) return "b";
    return f(n - 1) + f(n - 2);
}

int main() {
    for (int n = 0; n <= 10; n++) {
        cout << "f(" << n << ") = " << f(n) << endl;
    }
}
