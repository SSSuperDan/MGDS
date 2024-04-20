#ifndef MLGDENSEST_GDS_H
#define MLGDENSEST_GDS_H

#include "../Graphs/MLG.h"
#include "../Utils/StrOps.h"
#include "../Structs/IntHeap.h"
#include "../Structs/MixHeap.h"
#include "../Structs/SetHeap.h"

#define SMALL_VALUE 1e-6

double Greedy(MLG &mlg, vector<uint> &gds, const string &q_str, const string &p_str, bool use_heap = true);

double
Greedy_fast(MLG &mlg, vector<uint> &gds, const string &q_str, const string &p_str, double eps, bool use_heap = true);

template<class HEAP>
double Greedy_1qp(MLG &mlg, double q, double p, vector<uint> &gds); // Case 1: 1 <= q <= p

template<class HEAP>
double Greedy_1pq(MLG &mlg, double q, double p, vector<uint> &gds); // Case 2: 1 <= p <= q

template<class HEAP>
double
Greedy_1pq_fast(MLG &mlg, double q, double p, double eps, vector<uint> &gds); // Fast implementation of Greedy_1pq

template<class HEAP>
double Greedy_p1q(MLG &mlg, double q, double p, vector<uint> &gds); // Case 4: p <= 1 <= q

template<class HEAP>
double Greedy_min1q(MLG &mlg, double q, vector<uint> &gds); // Case 3: p = -inf

double Greedy_min1(MLG &mlg, vector<uint> &gds); // Special case: p = -inf, q = -inf

static inline double pow_sum(vector<uint> &deg_seq, double q) {
    double sum = 0;
    for (auto d: deg_seq) {
        sum += pow(d, q);
    }
    return sum;
}


static inline double deg_norm_pow_posp(vector<uint> &deg_seq, double q, double a) {
    return pow(pow_sum(deg_seq, q), a);
}

static inline double deg_norm_pow_negq(vector<uint> &deg_seq, double q, double a) {
    double sum = 0;
    for (auto d: deg_seq) {
        sum += (d ? pow(d, q) : pow(SMALL_VALUE, q));
    }
    return pow(sum, a);
}

static inline double deg_norm_pow(vector<uint> &deg_seq, double q, double a) {
    return pow(pow_sum(deg_seq, q), a);
}

static inline double deg_loc_norm_pow(vector<uint> &deg_seq, double a) {
    return pow((double) deg_seq.size(), a);
}

static inline pair<double, double> deg_norm_pow(vector<uint> &deg_seq, double q, double pa, double a) {
    double sum = pow_sum(deg_seq, q);
    return make_pair(pow(sum, pa), pow(sum, a));
}

#endif //MLGDENSEST_GDS_H
