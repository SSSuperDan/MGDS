#ifndef MLGDENSEST_GFIRMCORE_H
#define MLGDENSEST_GFIRMCORE_H

#include "Algo.h"
#include "../Graphs/MLG.h"
#include "../Utils/StrOps.h"
#include "../Structs/IntHeap.h"
#include "../Structs/MixHeap.h"
#include "../Structs/SetHeap.h"

struct DFC_INFO {
    uint k;
    uint size;
    double den;

    DFC_INFO(): k(-1), size(0), den(0) {};

    void Update(uint new_k, uint new_size, double new_den) {
        k = new_k;
        size = new_size;
        den = new_den;
    }
};

void UpdateDensestFC(DFC_INFO &densest_fc, vector<uint> &coreness, vector<uint> &dsg);

/** Densest FirmCore **/
DFC_INFO DensestFCLamb(MLG &mlg, vector<uint> &coreness, uint lamb, double beta);
double GetDen(vector<uint> & n_edges, uint n_vtx, double beta);
double DensestFC(MLG &mlg, vector<uint> &dsg, double beta);

/** Densest Generalized FirmCore **/
template<class HEAP>
        DFC_INFO DensestGFCLamb(MLG &mlg, vector<uint> &coreness, uint lamb, double p, double beta);
double GetGDen(vector<l_double> &n_edge_power, uint n_vtx, double p, double beta);
double DensestGFC(MLG &mlg, vector<uint> &dsg, const string &p, double beta, bool use_heap = true);

#endif //MLGDENSEST_GFIRMCORE_H
