#if MPFR

#include "GDSMultiPrecision.h"

double GreedyMp(MLG &mlg, vector<uint> &gds, const string &q_str, const string &p_str) {

    mpfr_set_default_prec(PREC);

    if (IsDigit(p_str)) {
        assert(IsDigit(q_str));

        auto p = stod(p_str);
        auto q = stod(q_str);
        assert(q >= 1);

        if (p >= q) return GreedyMp_1qp(mlg, q, p, gds);
        else if (p > 1) return GreedyMp_1pq(mlg, q, p, gds);
        else return GreedyMp_p1q(mlg, q, p, gds);

    } else {

        assert(p_str == "-inf");
        if (IsDigit(q_str)) {
            auto q = stod(q_str);
            return GreedyMp_min1q(mlg, q, gds);

        } else {
            assert(q_str == "-inf");
            return Greedy_min1(mlg, gds);
        }
    }
}

double GreedyMp_fast(MLG &mlg, vector<uint> &gds, const string &q_str, const string &p_str, double eps) {
    assert(IsDigit(q_str) && IsDigit(p_str));

    auto p = stod(p_str);
    auto q = stod(q_str);

    assert(q >= 1 && p >= 1);
    return GreedyMp_1pq_fast(mlg, q, p, eps, gds);
}

double GreedyMp_1qp(MLG &mlg, double q, double p, vector<uint> &gds) {

    uint n = mlg.GetN(), ln = mlg.GetLn(), obj_max_offset;
    vector<vector<uint>> deg;
    vector<uint> deg_pre(ln), deg_upd_v(ln), deg_upd(ln), deg_upd2(ln), del_seq(n);
    vector<bool> exists(n, true), aff(n, false);
    MixHeapMp heap(n);

    mpfr_t p_mpf, q_mpf, alpha, agg_deg_u, old_del, delta, ln_rec, layer_factor, obj, obj_max, obj_numerator, tmp;
    vector<mpfr_t> agg_deg(n);

    /** init multi-precision floats**/
    mpfr_inits(p_mpf, q_mpf, alpha, agg_deg_u, old_del, delta, ln_rec, layer_factor, obj, obj_max, obj_numerator, tmp,
               nullptr);
    mpfr_set_d(p_mpf, p, RND_PREC);
    mpfr_set_d(q_mpf, q, RND_PREC);
    mpfr_set_d(alpha, p / q, RND_PREC);
    mpfr_set_d(ln_rec, 1.0 / ln, RND_PREC);
    mpfr_pow(layer_factor, ln_rec, alpha, RND_PREC);

    /** compute aggregated deg and initialize obj_max **/
    mlg.GetDegree(deg);
    mpfr_set_d(obj_numerator, 0, RND_PREC);
    for (uint v = 0; v < n; v++) {
        mpfr_init(agg_deg[v]);
        deg_norm_pow(agg_deg[v], deg[v], q_mpf, alpha);
        //        mpfr_mul(agg_deg[v], tmp, layer_factor, RND_PREC);
        mpfr_add(obj_numerator, obj_numerator, agg_deg[v], RND_PREC);
    }

    mpfr_div_d(obj_max, obj_numerator, n, RND_PREC);
    obj_max_offset = 0;

    /** compute initiate score and create queue **/
    auto &nbrs = mlg.GetNbrs();
    for (uint v = 0; v < n; v++) {
        mpfr_set(value_mp[v], agg_deg[v], RND_PREC);
        for (auto &e: nbrs[v]) {
            deg_upd.assign(deg[e.nbr].begin(), deg[e.nbr].end());
            for (auto l: e.layer) {
                deg_upd[l] -= 1;
            }
            deg_norm_pow(tmp, deg_upd, q_mpf, alpha);
            //            mpfr_mul(tmp, tmp, layer_factor, RND_PREC);
            mpfr_sub(tmp, agg_deg[e.nbr], tmp, RND_PREC);
            mpfr_add(value_mp[v], value_mp[v], tmp, RND_PREC);
        }
    }
    heap.InsertAll();

    for (uint i = 0; i < n - 1; i++) {

        // select v with min score
        auto v = heap.RemoveMin();

        del_seq[i] = v;
        exists[v] = false;
        mpfr_sub(obj_numerator, obj_numerator, agg_deg[v], RND_PREC);

        print_debug_info(to_string(i) + " " + to_string(v));

        // update score of v's 2-hop neighbors
        vector<uint> affected_nbr;
        for (auto &e: nbrs[v]) {
            auto u = e.nbr;
            if (!exists[u]) continue;

            if (!aff[u]) {
                heap.Remove(u);
                affected_nbr.emplace_back(u);
                aff[u] = true;
            }

            deg_upd_v.assign(deg[v].begin(), deg[v].end());
            deg_pre.assign(deg[u].begin(), deg[u].end());
            for (auto l: e.layer) {
                deg[u][l]--;
                deg_upd_v[l]--;
            }

            deg_norm_pow(tmp, deg_upd_v, q_mpf, alpha);
            mpfr_sub(tmp, agg_deg[v], tmp, RND_PREC);
            mpfr_sub(value_mp[u], value_mp[u], tmp, RND_PREC);

            deg_norm_pow(agg_deg_u, deg[u], q_mpf, alpha);
            mpfr_sub(delta, agg_deg[u], agg_deg_u, RND_PREC);

            mpfr_sub(value_mp[u], value_mp[u], delta, RND_PREC);
            mpfr_sub(obj_numerator, obj_numerator, delta, RND_PREC);

            for (auto &e2: nbrs[u]) {
                auto w = e2.nbr;
                if (!exists[w]) continue;

                if (!aff[w]) {
                    heap.Remove(w);
                    affected_nbr.emplace_back(w);
                    aff[w] = true;
                }

                deg_upd.assign(deg_pre.begin(), deg_pre.end());
                deg_upd2.assign(deg[u].begin(), deg[u].end());

                for (auto l: e2.layer) {
                    deg_upd[l] -= 1;
                    deg_upd2[l] -= 1;
                }

                deg_norm_pow(tmp, deg_upd, q_mpf, alpha);
                mpfr_sub(old_del, agg_deg[u], tmp, RND_PREC);

                deg_norm_pow(tmp, deg_upd2, q_mpf, alpha);
                mpfr_sub(tmp, agg_deg_u, tmp, RND_PREC);
                mpfr_sub(tmp, old_del, tmp, RND_PREC);
                mpfr_sub(value_mp[w], value_mp[w], tmp, RND_PREC);
            }
            mpfr_set(agg_deg[u], agg_deg_u, RND_PREC);
        }

        for (auto u: affected_nbr) {
            heap.Insert(u);
            aff[u] = false;
        }

        /** compute the object func **/
        mpfr_div_d(obj, obj_numerator, n - i - 1, RND_PREC);
        if (mpfr_cmp(obj, obj_max) > 0) {
            mpfr_set(obj_max, obj, RND_PREC);
            obj_max_offset = i + 1;
        }
    }

    del_seq[n - 1] = heap.RemoveMin();
    gds.assign(del_seq.begin() + obj_max_offset, del_seq.end());
    mpfr_mul(obj_max, obj_max, layer_factor, RND_PREC);
    auto obj_max_db = mpfr_get_d(obj_max, RND_PREC);

    mpfr_clears(p_mpf, q_mpf, alpha, agg_deg_u, old_del, delta, ln_rec, layer_factor, obj, obj_max, obj_numerator, tmp, nullptr);
    for (uint v = 0; v < n; v++) {
        mpfr_clear(agg_deg[v]);
    }

    return pow(obj_max_db, 1 / p);
}

double GreedyMp_1pq(MLG &mlg, double q, double p, vector<uint> &gds) {

    uint n = mlg.GetN(), ln = mlg.GetLn(), obj_max_offset;

    vector<vector<uint>> deg;
    vector<uint> del_seq(n);
    vector<bool> exists(n, true), aff(n, false);
    MixHeapMp heap(n);

    mpfr_t p_mpf, q_mpf, q_rec, p_alpha, alpha, agg_deg_u, deriv_deg_u, old_del, delta, ln_rec, layer_factor, obj, obj_max, obj_numerator, tmp;
    vector<mpfr_t> agg_deg(n), deriv_deg(n);

    /** init multi-precision floats**/
    mpfr_inits(p_mpf, q_mpf, q_rec, p_alpha, alpha, agg_deg_u, deriv_deg_u, old_del, delta, ln_rec, layer_factor, obj,
               obj_max, obj_numerator, tmp, nullptr);
    mpfr_set_d(p_mpf, p, RND_PREC);
    mpfr_set_d(q_mpf, q, RND_PREC);
    mpfr_set_d(q_rec, 1 / q, RND_PREC);
    mpfr_set_d(alpha, p / q, RND_PREC);
    mpfr_set_d(ln_rec, 1.0 / ln, RND_PREC);
    mpfr_sub(p_alpha, alpha, q_rec, RND_PREC);
    mpfr_pow(layer_factor, ln_rec, alpha, RND_PREC);

    /** compute aggregated deg and initialize obj_max **/
    mlg.GetDegree(deg);
    mpfr_set_d(obj_numerator, 0, RND_PREC);
    for (uint v = 0; v < n; v++) {
        mpfr_init(agg_deg[v]);
        mpfr_init(deriv_deg[v]);

        deg_norm_pow(tmp, agg_deg[v], deg[v], q_mpf, p_alpha, alpha);
        mpfr_mul(deriv_deg[v], tmp, p_mpf, RND_PREC);
        mpfr_add(obj_numerator, obj_numerator, agg_deg[v], RND_PREC);
    }

    mpfr_div_d(obj_max, obj_numerator, n, RND_PREC);
    obj_max_offset = 0;

    /** compute initiate score and create queue **/
    auto &nbrs = mlg.GetNbrs();
    for (uint v = 0; v < n; v++) {
        mpfr_set(value_mp[v], agg_deg[v], RND_PREC);
        for (auto &e: nbrs[v]) {
            deg_loc_norm_pow(tmp, e.layer, q_rec);
            mpfr_mul(tmp, deriv_deg[e.nbr], tmp, RND_PREC);
            mpfr_add(value_mp[v], value_mp[v], tmp, RND_PREC);
        }
    }
    heap.InsertAll();

    for (uint i = 0; i < n - 1; i++) {

        // select v with min score
        auto v = heap.RemoveMin();

        del_seq[i] = v;
        exists[v] = false;
        mpfr_sub(obj_numerator, obj_numerator, agg_deg[v], RND_PREC);

        print_debug_info(to_string(i) + " " + to_string(v));

        // update score of v'v 2-hop neighbors
        vector<uint> affected_nbr;
        for (auto &e: nbrs[v]) {
            auto u = e.nbr;
            if (!exists[u]) continue;

            if (!aff[u]) {
                heap.Remove(u);
                affected_nbr.emplace_back(u);
                aff[u] = true;
            }

            for (auto l: e.layer) {
                deg[u][l]--;
            }

            deg_loc_norm_pow(tmp, e.layer, q_rec);
            mpfr_mul(tmp, deriv_deg[v], tmp, RND_PREC);
            mpfr_sub(value_mp[u], value_mp[u], tmp, RND_PREC);

            deg_norm_pow(tmp, agg_deg_u, deg[u], q_mpf, p_alpha, alpha);
            mpfr_mul(deriv_deg_u, tmp, p_mpf, RND_PREC);
            mpfr_sub(delta, agg_deg[u], agg_deg_u, RND_PREC);

            mpfr_sub(value_mp[u], value_mp[u], delta, RND_PREC);
            mpfr_sub(obj_numerator, obj_numerator, delta, RND_PREC);

            for (auto &e2: nbrs[u]) {
                auto w = e2.nbr;
                if (!exists[w]) continue;

                if (!aff[w]) {
                    heap.Remove(w);
                    affected_nbr.emplace_back(w);
                    aff[w] = true;
                }

                mpfr_sub(delta, deriv_deg[u], deriv_deg_u, RND_PREC);
                deg_loc_norm_pow(tmp, e2.layer, q_rec);
                mpfr_mul(tmp, delta, tmp, RND_PREC);
                mpfr_sub(value_mp[w], value_mp[w], tmp, RND_PREC);
            }

            mpfr_set(agg_deg[u], agg_deg_u, RND_PREC);
            mpfr_set(deriv_deg[u], deriv_deg_u, RND_PREC);
        }

        for (auto u: affected_nbr) {
            heap.Insert(u);
            aff[u] = false;
        }

        /** compute the object func **/
        mpfr_div_d(obj, obj_numerator, n - i - 1, RND_PREC);
        if (mpfr_cmp(obj, obj_max) > 0) {
            mpfr_set(obj_max, obj, RND_PREC);
            obj_max_offset = i + 1;
        }
    }

    del_seq[n - 1] = heap.RemoveMin();
    gds.assign(del_seq.begin() + obj_max_offset, del_seq.end());
    mpfr_mul(obj_max, obj_max, layer_factor, RND_PREC);
    auto obj_max_db = mpfr_get_d(obj_max, RND_PREC);

    mpfr_clears(p_mpf, q_mpf, q_rec, p_alpha, alpha, agg_deg_u, deriv_deg_u, old_del, delta, ln_rec, layer_factor, obj,
                obj_max, obj_numerator, tmp, nullptr);
    for (uint v = 0; v < n; v++) {
        mpfr_clear(agg_deg[v]);
        mpfr_clear(deriv_deg[v]);
    }

    return pow(obj_max_db, 1 / p);
}

double GreedyMp_1pq_fast(MLG &mlg, double q, double p, double eps, vector<uint> &gds) {

    uint n = mlg.GetN(), ln = mlg.GetLn(), obj_max_offset;

    vector<vector<uint>> deg;
    vector<uint> del_seq(n);
    vector<bool> exists(n, true), aff(n, false);
    MixHeapMp heap(n);

    mpfr_t p_mpf, q_mpf, q_rec, p_alpha, alpha, upd_factor, agg_deg_u, deg_norm, deriv_deg_u, old_del, delta, ln_rec, layer_factor, obj, obj_max, obj_numerator, tmp;
    vector<mpfr_t> agg_deg(n), deriv_deg(n), deg_norm_cpy(n);

    /** init multi-precision floats**/
    mpfr_inits(p_mpf, q_mpf, q_rec, p_alpha, alpha, upd_factor, agg_deg_u, deg_norm, deriv_deg_u, old_del, delta,
               ln_rec, layer_factor, obj, obj_max, obj_numerator, tmp, nullptr);
    mpfr_set_d(p_mpf, p, RND_PREC);
    mpfr_set_d(q_mpf, q, RND_PREC);
    mpfr_set_d(q_rec, 1 / q, RND_PREC);
    mpfr_set_d(alpha, p / q, RND_PREC);
    mpfr_set_d(ln_rec, 1.0 / ln, RND_PREC);
    mpfr_set_d(upd_factor, 1 + eps / (p - 1), RND_PREC);
    mpfr_sub(p_alpha, alpha, q_rec, RND_PREC);
    mpfr_pow(layer_factor, ln_rec, alpha, RND_PREC);

    mlg.GetDegree(deg);
    for (uint v = 0; v < n; v++) {
        mpfr_init(deg_norm_cpy[v]);
        deg_norm_pow(deg_norm_cpy[v], deg[v], q_mpf, q_rec);
    }

    /** compute aggregated deg and initialize obj_max **/
    mpfr_set_d(obj_numerator, 0, RND_PREC);
    for (uint v = 0; v < n; v++) {
        mpfr_init(agg_deg[v]);
        mpfr_init(deriv_deg[v]);

        deg_norm_pow(tmp, agg_deg[v], deg[v], q_mpf, p_alpha, alpha);
        mpfr_mul(deriv_deg[v], tmp, p_mpf, RND_PREC);
        mpfr_add(obj_numerator, obj_numerator, agg_deg[v], RND_PREC);
    }

    mpfr_div_d(obj_max, obj_numerator, n, RND_PREC);
    obj_max_offset = 0;

    /** compute initiate score and create queue **/
    auto &nbrs = mlg.GetNbrs();
    for (uint v = 0; v < n; v++) {
        mpfr_set(value_mp[v], agg_deg[v], RND_PREC);
        for (auto &e: nbrs[v]) {
            deg_loc_norm_pow(tmp, e.layer, q_rec);
            mpfr_mul(tmp, deriv_deg[e.nbr], tmp, RND_PREC);
            mpfr_add(value_mp[v], value_mp[v], tmp, RND_PREC);
        }
    }
    heap.InsertAll();

    for (uint i = 0; i < n - 1; i++) {

        // select v with min score
        auto v = heap.RemoveMin();

        del_seq[i] = v;
        exists[v] = false;
        mpfr_sub(obj_numerator, obj_numerator, agg_deg[v], RND_PREC);

        print_debug_info(to_string(i) + " " + to_string(v));

        // update score of v'v 2-hop neighbors
        vector<uint> affected_nbr;
        for (auto &e: nbrs[v]) {
            auto u = e.nbr;
            if (!exists[u]) continue;

            if (!aff[u]) {
                heap.Remove(u);
                affected_nbr.emplace_back(u);
                aff[u] = true;
            }

            for (auto l: e.layer) {
                deg[u][l]--;
            }

            deg_loc_norm_pow(tmp, e.layer, q_rec);
            mpfr_mul(tmp, deriv_deg[v], tmp, RND_PREC);
            mpfr_sub(value_mp[u], value_mp[u], tmp, RND_PREC);

            deg_norm_pow(tmp, agg_deg_u, deg[u], q_mpf, p_alpha, alpha);
            mpfr_mul(deriv_deg_u, tmp, p_mpf, RND_PREC);
            mpfr_sub(delta, agg_deg[u], agg_deg_u, RND_PREC);

            mpfr_sub(value_mp[u], value_mp[u], delta, RND_PREC);
            mpfr_sub(obj_numerator, obj_numerator, delta, RND_PREC);

            deg_norm_pow(deg_norm, deg[u], q_mpf, q_rec);
            mpfr_mul(tmp, upd_factor, deg_norm, RND_PREC);

            if (mpfr_cmp(deg_norm_cpy[u], tmp) > 0) {
                for (auto &e2: nbrs[u]) {
                    auto w = e2.nbr;
                    if (!exists[w]) continue;

                    if (!aff[w]) {
                        heap.Remove(w);
                        affected_nbr.emplace_back(w);
                        aff[w] = true;
                    }

                    mpfr_sub(delta, deriv_deg[u], deriv_deg_u, RND_PREC);
                    deg_loc_norm_pow(tmp, e2.layer, q_rec);
                    mpfr_mul(tmp, delta, tmp, RND_PREC);
                    mpfr_sub(value_mp[w], value_mp[w], tmp, RND_PREC);

                }
                mpfr_set(deg_norm_cpy[u], deg_norm, RND_PREC);
            }

            mpfr_set(agg_deg[u], agg_deg_u, RND_PREC);
            mpfr_set(deriv_deg[u], deriv_deg_u, RND_PREC);
        }

        for (auto u: affected_nbr) {
            heap.Insert(u);
            aff[u] = false;
        }

        /** compute the object func **/
        mpfr_div_d(obj, obj_numerator, n - i - 1, RND_PREC);
        if (mpfr_cmp(obj, obj_max) > 0) {
            mpfr_set(obj_max, obj, RND_PREC);
            obj_max_offset = i + 1;
        }
    }

    del_seq[n - 1] = heap.RemoveMin();
    gds.assign(del_seq.begin() + obj_max_offset, del_seq.end());
    mpfr_mul(obj_max, obj_max, layer_factor, RND_PREC);
    auto obj_max_db = mpfr_get_d(obj_max, RND_PREC);

    mpfr_clears(p_mpf, q_mpf, q_rec, p_alpha, alpha, upd_factor, agg_deg_u, deg_norm, deriv_deg_u, old_del, delta,
                ln_rec, layer_factor, obj, obj_max, obj_numerator, tmp, nullptr);
    for (uint v = 0; v < n; v++) {
        mpfr_clear(agg_deg[v]);
        mpfr_clear(deriv_deg[v]);
        mpfr_clear(deg_norm_cpy[v]);
    }

    return pow(obj_max_db, 1 / p);
}

double GreedyMp_p1q(MLG &mlg, double q, double p, vector<uint> &gds) {

    uint n = mlg.GetN(), ln = mlg.GetLn(), obj_max_offset;

    vector<vector<uint>> deg;
    vector<uint> del_seq(n), deg_sum(n);
    vector<bool> exists(n, true);
    MixHeapMp heap(n);

    mpfr_t p_mpf, q_mpf, p_rec, q_rec, alpha, ln_rec, layer_factor, agg_deg_u, score_u, obj, obj_max, obj_numerator, tmp;
    vector<mpfr_t> agg_deg(n);

    /** init multi-precision floats**/
    mpfr_inits(p_mpf, q_mpf, p_rec, q_rec, alpha, ln_rec, layer_factor, agg_deg_u, score_u, obj, obj_max, obj_numerator,
               tmp, nullptr);
    mpfr_set_d(p_mpf, p, RND_PREC);
    mpfr_set_d(q_mpf, q, RND_PREC);
    mpfr_set_d(alpha, p / q, RND_PREC);
    mpfr_set_d(p_rec, 1 / p, RND_PREC);
    mpfr_set_d(q_rec, 1 / q, RND_PREC);
    mpfr_set_d(ln_rec, 1.0 / ln, RND_PREC);
    mpfr_pow(layer_factor, ln_rec, q_rec, RND_PREC);

    /** compute aggregated deg and initiate score **/
    mlg.GetDegree(deg);
    mpfr_set_d(obj_numerator, 0, RND_PREC);

    for (uint v = 0; v < n; v++) {
        mpfr_init(agg_deg[v]);
        deg_norm_pow(value_mp[v], agg_deg[v], deg[v], q_mpf, q_rec, alpha);
        mpfr_add(obj_numerator, obj_numerator, agg_deg[v], RND_PREC);
        deg_sum[v] = sum(deg[v]);
    }
    heap.InsertAll();

    mpfr_div_d(obj_max, obj_numerator, n, RND_PREC);
    mpfr_pow(obj_max, obj_max, p_rec, RND_PREC);
    obj_max_offset = 0;

    uint i = 0;
    auto &nbrs = mlg.GetNbrs();
    while (i < n - 1) {

        // select v with min score
        auto v = heap.RemoveMin();

        del_seq[i++] = v;
        exists[v] = false;
        mpfr_sub(obj_numerator, obj_numerator, agg_deg[v], RND_PREC);

        print_debug_info(to_string(i) + " " + to_string(v));

        for (auto &e: nbrs[v]) {
            auto u = e.nbr;
            if (!exists[u]) continue;

            for (auto l: e.layer) {
                deg[u][l]--;
            }

            deg_sum[u] -= e.layer.size();

            if (!deg_sum[u]) {
                /* u is isolated due to the removal of v*/
                del_seq[i++] = u;
                exists[u] = false;
                heap.Remove(u);
                mpfr_sub(obj_numerator, obj_numerator, agg_deg[u], RND_PREC);
            } else {

                heap.Remove(u);

                deg_norm_pow(score_u, agg_deg_u, deg[u], q_mpf, q_rec, alpha);
                mpfr_sub(tmp, agg_deg[u], agg_deg_u, RND_PREC);
                mpfr_sub(obj_numerator, obj_numerator, tmp, RND_PREC);

                mpfr_set(agg_deg[u], agg_deg_u, RND_PREC);
                mpfr_set(value_mp[u], score_u, RND_PREC);
                heap.Insert(u);
            }
        }

        /** compute the object func **/
        if (i < n) {

            mpfr_div_d(obj, obj_numerator, n - i, RND_PREC);
            mpfr_pow(obj, obj, p_rec, RND_PREC);

            if (mpfr_cmp(obj, obj_max) > 0) {
                mpfr_set(obj_max, obj, RND_PREC);
                obj_max_offset = i;
            }
        }
    }

    gds.assign(del_seq.begin() + obj_max_offset, del_seq.end());
    mpfr_mul(obj_max, obj_max, layer_factor, RND_PREC);
    auto obj_max_db = mpfr_get_d(obj_max, RND_PREC);

    mpfr_clears(p_mpf, q_mpf, p_rec, q_rec, alpha, ln_rec, layer_factor, agg_deg_u, score_u, obj, obj_max, obj_numerator,
                tmp, nullptr);
    for (uint v = 0; v < n; v++) {
        mpfr_clear(agg_deg[v]);
    }

    return obj_max_db;
}

double GreedyMp_min1q(MLG &mlg, double q, vector<uint> &gds) {

    uint n = mlg.GetN(), ln = mlg.GetLn(), obj_max_offset;

    vector<vector<uint>> deg;
    vector<uint> del_seq(n);
    vector<bool> exists(n, true);
    MixHeapMp heap(n);

    mpfr_t q_mpf, q_rec, ln_rec, layer_factor, obj_max, tmp;

    mpfr_inits(q_mpf, q_rec, ln_rec, layer_factor, obj_max, tmp, nullptr);
    mpfr_set_d(q_mpf, q, RND_PREC);
    mpfr_set_d(q_rec, 1 / q, RND_PREC);
    mpfr_set_d(ln_rec, 1.0 / ln, RND_PREC);
    mpfr_pow(layer_factor, ln_rec, q_rec, RND_PREC);
    mpfr_set_d(obj_max, 0, RND_PREC);

    /** compute initiate score **/
    mlg.GetDegree(deg);
    for (uint v = 0; v < n; v++) {
        deg_norm_pow(value_mp[v], deg[v], q_mpf, q_rec);
    }
    heap.InsertAll();
    obj_max_offset = -1;

    auto &nbrs = mlg.GetNbrs();
    for (uint i = 0; i < n; i++) {
        // select v with min score
        auto v = heap.RemoveMin();

        del_seq[i] = v;
        if (obj_max_offset == -1 || mpfr_cmp(value_mp[v], obj_max) > 0) {
            mpfr_set(obj_max, value_mp[v], RND_PREC);
            obj_max_offset = i;
        }

        exists[v] = false;

        // update score of v'v 1-hop neighbors
        for (auto &e: nbrs[v]) {
            auto u = e.nbr;
            if (!exists[u]) continue;

            for (auto l: e.layer) {
                deg[u][l]--;
            }

            deg_norm_pow(tmp, deg[u], q_mpf, q_rec);
            heap.Update(u, tmp);
        }
    }

    gds.assign(del_seq.begin() + obj_max_offset, del_seq.end());
    mpfr_mul(obj_max, obj_max, layer_factor, RND_PREC);
    auto obj_max_db = mpfr_get_d(obj_max, RND_PREC);

    mpfr_clears(q_mpf, q_rec, ln_rec, layer_factor, obj_max, tmp, nullptr);

    return obj_max_db;
}

#endif