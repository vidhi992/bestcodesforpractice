#include <bits/stdc++.h>
using namespace std;

/*
Segment Tree with Lazy Propagation supporting:
 - Range sum query
 - Range add (increment)
 - Range assign (set to value)  <-- assignment has higher priority than add
 - Point update / query via wrappers

Usage:
  SegTree st(vec);                    // build from vector<long long>
  st.range_add(l, r, val);            // add val to every element in [l,r]
  st.range_assign(l, r, val);         // set every element in [l,r] to val
  long long s = st.range_sum(l, r);   // query sum on [l,r]
  st.point_update(pos, val);          // set single position to val
*/

struct SegTree {
    int n;
    struct Node {
        long long sum;
        long long add;      // lazy add
        long long assign;   // lazy assign (if hasAssign == true)
        bool hasAssign;
        Node(): sum(0), add(0), assign(0), hasAssign(false) {}
    };
    vector<Node> st;

    SegTree(int N = 0) { init(N); }
    SegTree(const vector<long long>& a) { init((int)a.size()); build(1, 0, n-1, a); }

    void init(int N) {
        n = N;
        st.assign(4 * max(1, n), Node());
    }

    void build(int p, int L, int R, const vector<long long>& a) {
        if (L == R) {
            st[p].sum = a[L];
            st[p].add = 0;
            st[p].hasAssign = false;
            return;
        }
        int m = (L + R) >> 1;
        build(p<<1, L, m, a);
        build(p<<1|1, m+1, R, a);
        pull(p);
    }

    void pull(int p) {
        st[p].sum = st[p<<1].sum + st[p<<1|1].sum;
    }

    // apply assign to node p for segment length len
    void apply_assign(int p, int len, long long val) {
        st[p].sum = val * len;
        st[p].assign = val;
        st[p].hasAssign = true;
        st[p].add = 0; // reset add because assign overwrites
    }

    // apply add to node p for segment length len
    void apply_add(int p, int len, long long val) {
        if (st[p].hasAssign) {
            // if node has pending assign, adjust assign value
            st[p].assign += val;
            st[p].sum += val * len;
        } else {
            st[p].add += val;
            st[p].sum += val * len;
        }
    }

    void push(int p, int L, int R) {
        if (L == R) return;
        int m = (L + R) >> 1;
        int left = p<<1, right = p<<1|1;

        if (st[p].hasAssign) {
            apply_assign(left, m - L + 1, st[p].assign);
            apply_assign(right, R - m, st[p].assign);
            st[p].hasAssign = false;
            st[p].assign = 0;
        }
        if (st[p].add != 0) {
            apply_add(left, m - L + 1, st[p].add);
            apply_add(right, R - m, st[p].add);
            st[p].add = 0;
        }
    }

    // range assign: set all in [i,j] to val
    void range_assign(int i, int j, long long val) { if (i>j) return; range_assign(1, 0, n-1, i, j, val); }
    void range_assign(int p, int L, int R, int i, int j, long long val) {
        if (i > R || j < L) return;
        if (i <= L && R <= j) {
            apply_assign(p, R - L + 1, val);
            return;
        }
        push(p, L, R);
        int m = (L + R) >> 1;
        range_assign(p<<1, L, m, i, j, val);
        range_assign(p<<1|1, m+1, R, i, j, val);
        pull(p);
    }

    // range add: add val to every element in [i,j]
    void range_add(int i, int j, long long val) { if (i>j) return; range_add(1, 0, n-1, i, j, val); }
    void range_add(int p, int L, int R, int i, int j, long long val) {
        if (i > R || j < L) return;
        if (i <= L && R <= j) {
            apply_add(p, R - L + 1, val);
            return;
        }
        push(p, L, R);
        int m = (L + R) >> 1;
        range_add(p<<1, L, m, i, j, val);
        range_add(p<<1|1, m+1, R, i, j, val);
        pull(p);
    }

    // range sum query [i,j]
    long long range_sum(int i, int j) { if (i>j) return 0; return range_sum(1, 0, n-1, i, j); }
    long long range_sum(int p, int L, int R, int i, int j) {
        if (i > R || j < L) return 0;
        if (i <= L && R <= j) return st[p].sum;
        push(p, L, R);
        int m = (L + R) >> 1;
        return range_sum(p<<1, L, m, i, j) + range_sum(p<<1|1, m+1, R, i, j);
    }

    // point update: set position pos to val
    void point_update(int pos, long long val) { point_update(1, 0, n-1, pos, val); }
    void point_update(int p, int L, int R, int pos, long long val) {
        if (L == R) {
            st[p].sum = val;
            st[p].add = 0;
            st[p].hasAssign = false;
            return;
        }
        push(p, L, R);
        int m = (L + R) >> 1;
        if (pos <= m) point_update(p<<1, L, m, pos, val);
        else point_update(p<<1|1, m+1, R, pos, val);
        pull(p);
    }

    // point query (value at position pos)
    long long point_query(int pos) { return point_query(1, 0, n-1, pos); }
    long long point_query(int p, int L, int R, int pos) {
        if (L == R) return st[p].sum;
        push(p, L, R);
        int m = (L + R) >> 1;
        if (pos <= m) return point_query(p<<1, L, m, pos);
        else return point_query(p<<1|1, m+1, R, pos);
    }
};

// ---------------- Example / quick tests ----------------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<long long> a = {1,2,3,4,5,6,7,8,9,10}; // size 10
    SegTree st(a);

    cout << "sum(0,9) = " << st.range_sum(0,9) << '\n'; // 55
    cout << "sum(2,5) = " << st.range_sum(2,5) << '\n'; // 3+4+5+6=18

    st.range_add(0, 4, 10); // add 10 to first 5 elements
    cout << "after add(0,4,10), sum(0,4) = " << st.range_sum(0,4) << '\n'; // (1..5)+5*10 = 15+50=65

    st.range_assign(3, 6, 5); // set positions 3..6 to 5
    cout << "after assign(3,6,5), sum(0,9) = " << st.range_sum(0,9) << '\n';

    st.point_update(0, 100);
    cout << "after point_update(0,100), a[0] = " << st.point_query(0) << '\n';
    cout << "final sum(0,9) = " << st.range_sum(0,9) << '\n';

    // stress test small random
    {
        mt19937 rng(123);
        int N = 100;
        vector<long long> b(N);
        for (int i=0;i<N;i++) b[i] = uniform_int_distribution<int>(0,100)(rng);
        SegTree t(b);
        // apply many random ops and compare with brute
        for (int it=0; it<1000; ++it) {
            int type = uniform_int_distribution<int>(0,3)(rng);
            int l = uniform_int_distribution<int>(0,N-1)(rng);
            int r = uniform_int_distribution<int>(l,N-1)(rng);
            if (type == 0) { // range add
                int val = uniform_int_distribution<int>(-50,50)(rng);
                t.range_add(l,r,val);
                for (int i=l;i<=r;i++) b[i]+=val;
            } else if (type==1) { // range assign
                int val = uniform_int_distribution<int>(-20,120)(rng);
                t.range_assign(l,r,val);
                for (int i=l;i<=r;i++) b[i]=val;
            } else if (type==2) { // point update
                int pos = uniform_int_distribution<int>(0,N-1)(rng);
                int val = uniform_int_distribution<int>(-100,200)(rng);
                t.point_update(pos,val); b[pos]=val;
            } else { // range query
                long long s1 = t.range_sum(l,r);
                long long s2 = 0;
                for (int i=l;i<=r;i++) s2+=b[i];
                if (s1 != s2) {
                    cerr << "Mismatch! expected " << s2 << " got " << s1 << " for ["<<l<<","<<r<<"]\n";
                    return 0;
                }
            }
        }
        cout << "randomized tests passed\n";
    }

    return 0;
}
