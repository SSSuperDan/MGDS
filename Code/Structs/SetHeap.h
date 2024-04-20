#ifndef MLGDENSEST_SETHEAP_H
#define MLGDENSEST_SETHEAP_H


#include "../Header.h"
#include "../Utils/ArrOps.h"
#include "RealHeap.h"

class SetHeap: public RealHeap<double> {

public:
    explicit SetHeap(uint n_);
    ~SetHeap() = default;

    void InsertAll() override;
    void Insert(uint v) override;
    void Remove(uint v) override;
    void Update(uint v, double value) override;
    uint RemoveMin() override;

private:
    struct cmp {
        bool operator()(const uint &a, const uint &b) const {
            return value[a] < value[b] || (value[a] == value[b] && a < b);
        }
    };

    set<uint, cmp> value_srt;
};

#endif //MLGDENSEST_SETHEAP_H
