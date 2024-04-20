#ifndef MLGDENSEST_MIXHEAPMP_H
#define MLGDENSEST_MIXHEAPMP_H

#if MPFR
#include "../Header.h"
#include "../Utils/ArrOps.h"
#include "RealHeap.h"

extern mpfr_t* value_mp;

class MixHeapMp {

public:
    explicit MixHeapMp(uint n_);
    ~MixHeapMp();

    void InsertAll();
    void Insert(uint v);
    void Remove(uint v);
    void Update(uint v, mpfr_t &new_value);
    uint RemoveMin();

    void PrtHeap();

private:

    struct ele_cmp {
        bool operator()(const uint &a, const uint &b) const {
            auto res = mpfr_cmp(value_mp[a], value_mp[b]);
            return res < 0 || (!res && a < b);
        }
    };

    uint *pre;
    uint *next;

    set<uint> bins;
    set<uint, ele_cmp> min_bin;
    unordered_map<uint, uint> bin_heads;

    uint min_bin_value{};
    mpfr_t ui{};
    uint n_vert;

    void BuildMinBin();
    void Insert2Bin(uint v, uint v_value);

    uint ToUint(mpfr_t val) {
        assert (mpfr_fits_uint_p(val, RND_PREC));
        mpfr_rint_floor(ui, val, RND_PREC);
        return uint(mpfr_get_ui(ui, RND_PREC));
    }
};

#endif

#endif //MLGDENSEST_MIXHEAPMP_H
