#ifndef MLGDENSEST_ARROPS_H
#define MLGDENSEST_ARROPS_H

#include "../Header.h"

static uint sum(vector<uint> &arr) {
    uint sum = 0;
    for (auto val:arr) {
        sum += val;
    }
    return sum;
}

template<class T>
        static T min(const vector<T> &arr) {
    auto val = *(arr.begin());
    for (auto iter = arr.begin() + 1; iter != arr.end(); iter++) {
        if (*iter < val) {
            val = *iter;
        }
    }
    return val;
}

template<class T>
        static string Arr2Str(const T *arr, uint length) {
    uint i;
    string s = "[";

    for (i = 0; i < length - 1; i++) {
        s += to_string(arr[i]) + ",";
    }

    if (length) s += to_string(arr[i]);

    s += "]";

    return s;  // format: [1,2,3,4]
}

template<class T>
        static string Vec2Str(const vector<T> &arr) {
    uint i, length = arr.size();

    string s = "[";
    for (i = 0; i < length - 1; i++) {
        s += to_string(arr[i]) + ",";
    }
    if (length) s += to_string(arr[i]);

    s += "]";

    return s;
}

template<class T>
        static string Set2Str(const set<T> &arr) {
    uint i = 0, length = arr.size();

    string s = "[";
    for (auto v:arr) {
        i += 1;
        if (i < length) s += to_string(v) + ",";
        else s += to_string(v);
    }

    s += "]";

    return s;
}

template<class T, class C>
        static string Set2Str(const set<T, C> &arr) {
    uint i = 0, length = arr.size();

    string s = "[";
    for (auto v:arr) {
        i += 1;
        if (i < length) s += to_string(v) + ",";
        else s += to_string(v);
    }

    s += "]";

    return s;
}


#endif //MLGDENSEST_ARROPS_H
