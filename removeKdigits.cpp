#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    string removeKdigits(string num, int k) {
        stack<char> st;
        for (char c : num) {
            while (!st.empty() && k > 0 && st.top() > c) {
                st.pop();
                k--;
            }
            st.push(c);
        }

        while (k > 0 && !st.empty()) {
            st.pop();
            k--;
        }

        string res = "";
        while (!st.empty()) {
            res += st.top();
            st.pop();
        }

        reverse(res.begin(), res.end());

        while (!res.empty() && res[0] == '0') {
            res.erase(res.begin());
        }

        return res.empty() ? "0" : res;
    }
};

int main() {
    Solution sol;
    string num;
    int k;

    cout << "Enter number string: ";
    cin >> num;
    cout << "Enter value of k: ";
    cin >> k;

    string result = sol.removeKdigits(num, k);
    cout << "Smallest possible number: " << result << endl;

    return 0;
}
