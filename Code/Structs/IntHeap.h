#ifndef MLGDENSEST_INTHEAP_H
#define MLGDENSEST_INTHEAP_H


#include "../Header.h"

class IntHeap{

public:
    IntHeap(uint n_, uint n_bin_);
    ~IntHeap();

    void Insert(uint v, uint value);
    void Remove(uint v, uint old_value);
    void Update(uint v, uint old_value, uint value);

    void GetVtx(uint value, vector<uint>& vtx);
    uint GetMin(vector<uint>& vtx);
    void PrtHeap();

    uint SelectMin() {
        Tighten();
        assert(!Empty());
        return bin[min_val];
    }

private:
    inline void Tighten() {
        while (min_val <= max_val && bin[min_val] == -1) ++min_val;
        while (min_val <= max_val && bin[max_val] == -1) --max_val;
    }

    inline bool Empty() {
        Tighten();
        return min_val > max_val;
    }

    uint *pre;
    uint *next;
    uint *bin;

    uint n_vert;
    uint n_bin;
    uint max_val{0}, min_val{0};
};


#endif //MLGDENSEST_INTHEAP_H
