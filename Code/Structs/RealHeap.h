#ifndef MLGDENSEST_REALHEAP_H
#define MLGDENSEST_REALHEAP_H


#include "../Header.h"

template<class D>
        class RealHeap {

public:
    explicit RealHeap(uint n_):n_vert(n_){
        value.resize(n_);
    };

    ~RealHeap() = default;

    virtual void InsertAll() = 0;
    virtual void Insert(uint v) = 0;
    virtual void Remove(uint v) = 0;
    virtual void Update(uint v, D value) = 0;
    virtual uint RemoveMin() = 0;

    vector<D>& GetValue() {
        return value;
    }

protected:
    static vector<D> value;
    uint n_vert;
};

template <class D>
        vector<D> RealHeap<D>::value;


#endif //MLGDENSEST_REALHEAP_H
