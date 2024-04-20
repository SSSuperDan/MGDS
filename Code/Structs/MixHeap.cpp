#include "MixHeap.h"

MixHeap::MixHeap(uint n_) : RealHeap(n_), min_bin_value(UINT_MAX) {
    pre = new uint[n_vert];
    next = new uint[n_vert];
}

MixHeap::~MixHeap() {
    delete[] pre;
    delete[] next;
}

void MixHeap::InsertAll() {
    for (uint v = 0; v < n_vert; v++) {
        auto v_value = uint(value[v]);
        Insert2Bin(v, v_value);
    }
    BuildMinBin();
}

void MixHeap::Insert(uint v) {
    auto v_value = uint(value[v]);

    // insert v at the min bin
    if (v_value <= min_bin_value) {
        min_bin.insert(v);
        return;
    }

    Insert2Bin(v, v_value);
}

void MixHeap::Remove(uint v) {
    auto v_value = uint(value[v]);
    if (v_value <= min_bin_value) {
        min_bin.erase(v);
    } else {
        auto iter = bin_heads.find(v_value);
        if (iter->second == v) {
            if (next[v] == -1) {
                bins.erase(v_value);
                bin_heads.erase(v_value);
            } else {
                bin_heads[v_value] = next[v];
                pre[next[v]] = -1;
            }
        } else {
            auto pv = pre[v];
            next[pv] = next[v];
            if (next[v] != -1) pre[next[v]] = pv;
        }
    }
}

void MixHeap::Update(uint v, double new_value) {
    auto v_value = uint(value[v]);
    if (v_value != min_bin_value && v_value == uint(new_value)) {
        return;
    }

    Remove(v);
    value[v] = new_value;
    Insert(v);
}

uint MixHeap::RemoveMin() {
    if (min_bin.empty()) BuildMinBin();

    auto min = *(min_bin.begin());
    min_bin.erase(min);
    return min;
}

void MixHeap::BuildMinBin() {
    min_bin_value = *(bins.begin());
    auto v = bin_heads[min_bin_value];
    while (v != -1) {
        min_bin.insert(v);
        v = next[v];
    }
    bins.erase(min_bin_value);
    bin_heads.erase(min_bin_value);
}

void MixHeap::Insert2Bin(uint v, uint v_value) {
    auto iter = bins.find(v_value);
    if (iter == bins.end()) {
        bins.insert(v_value);
        bin_heads[v_value] = v;
        pre[v] = -1;
        next[v] = -1;

        if (v_value < min_bin_value) {
            min_bin_value = v_value;
        }

    } else {
        auto head = bin_heads[v_value];
        next[v] = head;
        pre[v] = -1;
        pre[head] = v;

        bin_heads[v_value] = v;
    }
}

void MixHeap::PrtHeap() {
    uint curr, cnt, tot_cnt = min_bin.size();

    cout << "MIN BIN : " << Set2Str(min_bin) << endl;
    for (auto val : bins) {
        cout << "BIN " << val << " : ";
        cnt = 0;
        curr = bin_heads[val];
        while (curr != -1) {
            cnt++;
            cout << curr << "-->";
            curr = next[curr];
        }
        cout << "-1 (" << cnt << ")" << endl;
        tot_cnt += cnt;
    }
    cout << "Tot cnt = " << tot_cnt << "." << endl;
    cout << "============================" << endl;
}
