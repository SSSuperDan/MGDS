#ifndef MLGDENSEST_MIXHEAP_H
#define MLGDENSEST_MIXHEAP_H

#include "../Header.h"
#include "../Utils/ArrOps.h"
#include "RealHeap.h"

class MixHeap: public RealHeap<double> {

public:
    explicit MixHeap(uint n_);
    ~MixHeap();

    void InsertAll() override;
    void Insert(uint v) override;
    void Remove(uint v) override;
    void Update(uint v, double value) override;
    uint RemoveMin() override;

    void PrtHeap();

private:

    struct ele_cmp {
        bool operator()(const uint &a, const uint &b) const {
            return value[a] < value[b] || (value[a] == value[b] && a < b);
        }
    };

    uint *pre;
    uint *next;

    set<uint> bins;
    set<uint, ele_cmp> min_bin;
    unordered_map<uint, uint> bin_heads;

    uint min_bin_value{};

    void BuildMinBin();
    void Insert2Bin(uint v, uint v_value);
};


#endif //MLGDENSEST_MIXHEAP_H
