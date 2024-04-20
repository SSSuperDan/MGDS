#include "IntHeap.h"

IntHeap::IntHeap(uint n_, uint n_bin_) : n_vert(n_), n_bin(n_bin_) {
    pre = new uint[n_vert];
    next = new uint[n_vert];
    bin = new uint[n_bin + 1];

    min_val = n_bin;
    max_val = 0;

    memset(bin, -1, (n_bin + 1) * sizeof(uint));
}

IntHeap::~IntHeap() {
    delete[] pre;
    delete[] next;
    delete[] bin;
}

void IntHeap::Insert(uint v, uint value) {
    uint head;

    head = bin[value];
    pre[v] = -1;
    next[v] = head;
    bin[value] = v;
    if (head != -1) pre[head] = v;

    if (value < min_val) min_val = value;
    if (value > max_val) max_val = value;
}

void IntHeap::Remove(uint v, uint old_value) {
    if (pre[v] == -1) { // v is the first element of the bin
        bin[old_value] = next[v];
        if (next[v] != -1) pre[next[v]] = -1;
    } else { // v is not the first element
        uint pv = pre[v];
        next[pv] = next[v];
        if (next[v] != -1) pre[next[v]] = pv;
    }
}

void IntHeap::Update(uint v, uint old_value, uint value) {
    Remove(v, old_value);
    Insert(v, value);
}

void IntHeap::GetVtx(uint value, vector<uint>& vtx) {
    if (value < min_val || value > max_val || bin[value] == -1) return;

    uint curr = bin[value];
    while (curr != -1) {
        vtx.emplace_back(curr);
        curr = next[curr];
    }

    bin[value] = -1;
}

uint IntHeap::GetMin(vector<uint> &vtx) {
    Tighten();
    GetVtx(min_val, vtx);
    return min_val;
}

void IntHeap::PrtHeap() {
    uint curr, cnt;

    for (uint i = min_val; i <= max_val; i++) {
        if (bin[i] != -1) {
            cout << "[" << i << "] : ";

            cnt = 0;
            curr = bin[i];
            while (curr != -1) {
                cnt ++;
                cout << curr << "-->";
                curr = next[curr];
            }
            cout << "-1 (" << cnt << ")" << endl;
        }
    }
    cout << "=========" << endl;
}
