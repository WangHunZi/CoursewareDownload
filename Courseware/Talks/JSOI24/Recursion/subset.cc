#include <iostream>
#include <vector>
#include <string>
using namespace std;

using Subsets = vector<vector<string>>;

Subsets subsets(const vector<string>& S) {
    if (S.size() == 0) {
        return { {} };
    } else {
        string x = S.back();
        auto S1 = S;
        S1.pop_back();  // S1 = S \ {x}

        Subsets res;

        for (auto& subset : subsets(S1)) {
            auto subset_with_x = subset;
            subset_with_x.push_back(x);

            res.push_back(subset);
            res.push_back(subset_with_x);
        }
        return res;
    }
}

void render(const auto& subsets) {
    for (auto& subset : subsets) {
        cout << "{";
        for (auto& x : subset) {
            cout << " " << x;
        }
        cout << " }" << endl;
    }
    cout << endl;
}

int main() {
    render(subsets({"1", "2"}));
    render(subsets({"1", "2", "3"}));
    render(subsets({"red", "yellow", "green", "blue"}));
}
