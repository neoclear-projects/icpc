#include <bits/stdc++.h>
#include <algorithm>

#define inf 0xffff
typedef long long i64;
typedef double f64;
#define nil NULL

using namespace std;

namespace io {
    inline i64 read_i64() {
        i64 ans = 0, f = 1;
        int ch = getchar();
        while (!isdigit(ch)) {
            if (ch == '-') { f = -1; }
            ch = getchar();
        }
        while (isdigit(ch)) {
            ans = ans * 10 + ch - '0';
            ch = getchar();
        }
        return ans * f;
    }

    inline f64 read_f64() {
        f64 ans = 0.0;
        f64 divide = 1.0;
        bool flag = false;
        f64 f = 1.0;
        int ch = getchar();
        while (!isdigit(ch)) {
            if (ch == '-') { f = -1.0; }
            ch = getchar();
        }
        while (isdigit(ch) || ch == '.') {
            if (ch == '.') {
                ch = getchar();
                flag = true;
            }
            ans = ans * 10.0 + static_cast<f64>((ch - '0'));
            if (flag) { divide *= 10.0; }
            ch = getchar();
        }
        return ans * f / divide;
    }

    inline string read_str() {
        string ans;
        char ch = getchar();
        while (isspace(ch)) { ch = getchar(); }
        ans.push_back(ch);
        while (!isspace(ch = getchar())) { ans.push_back(ch); }
        return ans;
    }
}

inline void init() {
    freopen("in.txt", "r", stdin);
    // freopen("out.txt", "w", stdout);
}

int main() {
    #ifdef __TESTING__
    init();
    #endif

    return 0;
}
