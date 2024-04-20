#include "SetHeap.h"

SetHeap::SetHeap(uint n_) : RealHeap(n_) {}

void SetHeap::InsertAll() {
    for (uint v = 0; v < n_vert; v++) {
        value_srt.insert(v);
    }
}

void SetHeap::Insert(uint v) {
    value_srt.insert(v);
}

void SetHeap::Remove(uint v) {
    value_srt.erase(v);
}

void SetHeap::Update(uint v, double new_value) {
    Remove(v);
    value[v] = new_value;
    Insert(v);
}

uint SetHeap::RemoveMin() {
    auto v = *(value_srt.begin());
    value_srt.erase(v);
    return v;
}