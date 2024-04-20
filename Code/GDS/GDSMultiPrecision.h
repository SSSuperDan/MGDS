#ifndef MLGDENSEST_GDSMULTIPRECISION_H
#define MLGDENSEST_GDSMULTIPRECISION_H

#if MPFR
#include "GDS.h"
#include "../Graphs/MLG.h"
#include "../Utils/StrOps.h"
#include "../Structs/IntHeap.h"
#include "../Structs/MixHeapMp.h"

double GreedyMp(MLG &mlg, vector<uint> &gds, const string &q_str, const string &p_str);

double GreedyMp_fast(MLG &mlg, vector<uint> &gds, const string &q_str, const string &p_str, double eps);

double GreedyMp_1qp(MLG &mlg, double q, double p, vector<uint> &gds); // Case 1: 1 <= q <= p

double GreedyMp_1pq(MLG &mlg, double q, double p, vector<uint> &gds); // Case 2: 1 <= p <= q

double GreedyMp_1pq_fast(MLG &mlg, double q, double p, double eps, vector<uint> &gds); // Fast implementation of Greedy_1pq

double GreedyMp_p1q(MLG &mlg, double q, double p, vector<uint> &gds); // Case 4: p <= 1 <= q

double GreedyMp_min1q(MLG &mlg, double q, vector<uint> &gds); // Case 3: p = -inf

static inline void pow_sum(mpfr_t &res, vector<uint> &deg_seq, mpfr_t &q) {
    mpfr_set_d(res, 0, RND_PREC);

    mpfr_t power;
    mpfr_init(power);

    for (auto d: deg_seq) {
        mpfr_ui_pow(power, d, q, RND_PREC);
        mpfr_add(res, res, power, RND_PREC);
    }

    mpfr_clear(power);
}

static inline void deg_norm_pow(mpfr_t &res, vector<uint> &deg_seq, mpfr_t &q, mpfr_t &a) {
    pow_sum(res, deg_seq, q);
    mpfr_pow(res, res, a, RND_PREC);
}

static inline void deg_loc_norm_pow(mpfr_t &res, vector<uint> &deg_seq, mpfr_t &a) {
    mpfr_ui_pow(res, deg_seq.size(), a, RND_PREC);
}

static inline void deg_norm_pow(mpfr_t &res_1, mpfr_t &res_2, vector<uint> &deg_seq, mpfr_t &q, mpfr_t &pa, mpfr_t &a) {
    pow_sum(res_1, deg_seq, q);
    mpfr_pow(res_2, res_1, a, RND_PREC);
    mpfr_pow(res_1, res_1, pa, RND_PREC);
}

static void print(mpfr_t &val) {
    print(to_string(mpfr_get_d(val, RND_PREC)));
}
#endif
#endif //MLGDENSEST_GDSMULTIPRECISION_H
