#include <iostream>
#include <cmath>
using namespace std;

const int n = 10;
int a[n] = {3, 1, 4, 5};
int b[n] = {6, 7, 2, 8};

int main() {
  int s1 = 0;
  for (int i = 1; i <= n; i++) {
    int s2 = 0;
    for (int j = 1; j <= n; j++)
      s2 += a[i] * b[j];
    s1 += s2;
  }
  cout << s1 << endl;
}
