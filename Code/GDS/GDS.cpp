#include "GDS.h"

double Greedy(MLG &mlg, vector<uint> &gds, const string &q_str, const string &p_str, bool use_heap) {

    if (IsDigit(p_str)) {
        assert(IsDigit(q_str));

        auto p = stod(p_str);
        auto q = stod(q_str);
        assert(q >= 1);

        if (p >= q) return use_heap ? Greedy_1qp<MixHeap>(mlg, q, p, gds) : Greedy_1qp<SetHeap>(mlg, q, p, gds);
        else if (p > 1) return use_heap ? Greedy_1pq<MixHeap>(mlg, q, p, gds) : Greedy_1pq<SetHeap>(mlg, q, p, gds);
        else return use_heap ? Greedy_p1q<MixHeap>(mlg, q, p, gds) : Greedy_p1q<SetHeap>(mlg, q, p, gds);

    } else {

        assert(p_str == "-inf");
        if (IsDigit(q_str)) {
            auto q = stod(q_str);
            return use_heap ? Greedy_min1q<MixHeap>(mlg, q, gds) : Greedy_min1q<SetHeap>(mlg, q, gds);

        } else {
            assert(q_str == "-inf");
            return Greedy_min1(mlg, gds);
        }
    }
}

double Greedy_fast(MLG &mlg, vector<uint> &gds, const string &q_str, const string &p_str, double eps, bool use_heap) {
    assert(IsDigit(q_str) && IsDigit(p_str));

    auto p = stod(p_str);
    auto q = stod(q_str);

    assert(q >= 1 && p >= 1);
    return use_heap ? Greedy_1pq_fast<MixHeap>(mlg, q, p, eps, gds) : Greedy_1pq_fast<SetHeap>(mlg, q, p, eps, gds);
}

template<class HEAP>
        double Greedy_1qp(MLG &mlg, double q, double p, vector<uint> &gds) {

    uint n = mlg.GetN(), ln = mlg.GetLn(), obj_max_offset;
    double alpha = p / q, obj, obj_max;
    l_double obj_numerator = 0;

    vector<vector<uint>> deg;
    vector<uint> deg_pre(ln), deg_upd_v(ln), deg_upd(ln), deg_upd2(ln), del_seq(n);
    vector<bool> exists(n, true), aff(n, false);
    vector<double> agg_deg(n);
    HEAP heap(n);

    /** compute aggregated deg and initialize obj_max **/
    mlg.GetDegree(deg);
    for (uint v = 0; v < n; v++) {
        agg_deg[v] = deg_norm_pow(deg[v], q, alpha);
        obj_numerator += agg_deg[v];
    }

    obj_max = double(obj_numerator / n);
    obj_max_offset = 0;

    /** compute initiate score and create queue **/
    auto &value = heap.GetValue();
    auto &nbrs = mlg.GetNbrs();
    for (uint v = 0; v < n; v++) {
        value[v] = agg_deg[v];
        for (auto &e: nbrs[v]) {
            deg_upd.assign(deg[e.nbr].begin(), deg[e.nbr].end());
            for (auto l: e.layer) {
                deg_upd[l] -= 1;
            }
            value[v] += agg_deg[e.nbr] - deg_norm_pow(deg_upd, q, alpha);
        }
    }
    heap.InsertAll();

    for (uint i = 0; i < n - 1; i++) {

        // select v with min score
        auto v = heap.RemoveMin();

        del_seq[i] = v;
        exists[v] = false;
        obj_numerator -= agg_deg[v];

        print_debug_info(
                to_string(i) + " " + to_string(v) + " " + to_string(value[v]) + " " + to_string(obj_numerator));

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

            auto agg_deg_v = deg_norm_pow(deg_upd_v, q, alpha);
            value[u] -= agg_deg[v] - agg_deg_v;

            auto agg_deg_u = deg_norm_pow(deg[u], q, alpha);
            auto delta = agg_deg[u] - agg_deg_u;

            value[u] -= delta;
            obj_numerator -= delta;

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

                auto old_del = agg_deg[u] - deg_norm_pow(deg_upd, q, alpha);
                auto new_del = agg_deg_u - deg_norm_pow(deg_upd2, q, alpha);
                value[w] -= old_del - new_del;
            }

            agg_deg[u] = agg_deg_u;
        }

        for (auto u: affected_nbr) {
            heap.Insert(u);
            aff[u] = false;
        }

        /** compute the object func **/
        obj = double(obj_numerator / (n - i - 1));

        if (obj > obj_max) {
            obj_max = obj;
            obj_max_offset = i + 1;
        }
    }

    del_seq[n - 1] = heap.RemoveMin();
    gds.assign(del_seq.begin() + obj_max_offset, del_seq.end());
    return pow(obj_max * pow(1.0 / ln, alpha), 1 / p);
}

template<class HEAP>
        double Greedy_1pq(MLG &mlg, double q, double p, vector<uint> &gds) {

    double q_rec = 1 / q, alpha = p * q_rec, p_alpha = alpha - q_rec, obj, obj_max;
    l_double obj_numerator = 0;
    uint n = mlg.GetN(), ln = mlg.GetLn(), obj_max_offset;

    vector<vector<uint>> deg;
    vector<uint> del_seq(n);
    vector<bool> exists(n, true), aff(n, false);
    vector<double> agg_deg(n), deriv_deg(n);
    HEAP heap(n);

    /** compute aggregated deg and initialize obj_max **/
    mlg.GetDegree(deg);
    for (uint v = 0; v < n; v++) {
        auto deg_norms = deg_norm_pow(deg[v], q, p_alpha, alpha);
        deriv_deg[v] = deg_norms.first * p;
        agg_deg[v] = deg_norms.second;
        obj_numerator += agg_deg[v];
    }

    obj_max = double(obj_numerator / n);
    obj_max_offset = 0;

    /** compute initiate score and create queue **/
    auto &value = heap.GetValue();
    auto &nbrs = mlg.GetNbrs();
    for (uint v = 0; v < n; v++) {
        value[v] = agg_deg[v];
        for (auto &e: nbrs[v]) {
            value[v] += deriv_deg[e.nbr] * deg_loc_norm_pow(e.layer, q_rec);
        }
    }
    heap.InsertAll();

    for (uint i = 0; i < n - 1; i++) {

        // select v with min score
        auto v = heap.RemoveMin();

        del_seq[i] = v;
        exists[v] = false;
        obj_numerator -= agg_deg[v];

        print_debug_info(
                to_string(i) + " " + to_string(v) + " " + to_string(value[v]) + " " + to_string(obj_numerator));

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

            value[u] -= deriv_deg[v] * deg_loc_norm_pow(e.layer, q_rec);

            auto deg_norms = deg_norm_pow(deg[u], q, p_alpha, alpha);
            auto deriv_deg_u = deg_norms.first * p;
            auto agg_deg_u = deg_norms.second;
            auto delta = agg_deg[u] - agg_deg_u;
            value[u] -= delta;
            obj_numerator -= delta;

            for (auto &e2: nbrs[u]) {
                auto w = e2.nbr;
                if (!exists[w]) continue;

                if (!aff[w]) {
                    heap.Remove(w);
                    affected_nbr.emplace_back(w);
                    aff[w] = true;
                }

                value[w] -= (deriv_deg[u] - deriv_deg_u) * deg_loc_norm_pow(e2.layer, q_rec);
            }

            agg_deg[u] = agg_deg_u;
            deriv_deg[u] = deriv_deg_u;
        }

        for (auto u: affected_nbr) {
            heap.Insert(u);
            aff[u] = false;
        }

        /** compute the object func **/
        obj = double(obj_numerator / (n - i - 1));
        if (obj > obj_max) {
            obj_max = obj;
            obj_max_offset = i + 1;
        }
    }

    del_seq[n - 1] = heap.RemoveMin();
    gds.assign(del_seq.begin() + obj_max_offset, del_seq.end());
    return pow(obj_max * pow(1.0 / ln, alpha), 1 / p);
}

template<class HEAP>
        double Greedy_1pq_fast(MLG &mlg, double q, double p, double eps, vector<uint> &gds) {

    double q_rec = 1 / q, alpha = p * q_rec, p_alpha = alpha - q_rec, upd_factor = 1 + eps / (p - 1), obj, obj_max;
    l_double obj_numerator = 0;
    uint n = mlg.GetN(), ln = mlg.GetLn(), obj_max_offset;

    vector<vector<uint>> deg;
    vector<uint> del_seq(n);
    vector<bool> exists(n, true), aff(n, false);
    vector<double> agg_deg(n), deriv_deg(n), deg_norm_cpy(n);
    HEAP heap(n);

    mlg.GetDegree(deg);
    for (uint v = 0; v < n; v++) {
        deg_norm_cpy[v] = deg_norm_pow(deg[v], q, q_rec);
    }

    /** compute aggregated deg and initialize obj_max **/
    for (uint v = 0; v < n; v++) {
        auto deg_norms = deg_norm_pow(deg[v], q, p_alpha, alpha);
        deriv_deg[v] = deg_norms.first * p;
        agg_deg[v] = deg_norms.second;
        obj_numerator += agg_deg[v];
    }

    obj_max = double(obj_numerator / n);
    obj_max_offset = 0;

    /** compute initiate score and create queue **/
    auto &value = heap.GetValue();
    auto &nbrs = mlg.GetNbrs();
    for (uint v = 0; v < n; v++) {
        value[v] = agg_deg[v];
        for (auto &e: nbrs[v]) {
            value[v] += deriv_deg[e.nbr] * deg_loc_norm_pow(e.layer, q_rec);
        }
    }
    heap.InsertAll();

    for (uint i = 0; i < n - 1; i++) {

        // select v with min score
        auto v = heap.RemoveMin();

        del_seq[i] = v;
        exists[v] = false;
        obj_numerator -= agg_deg[v];

        print_debug_info(
                to_string(i) + " " + to_string(v) + " " + to_string(value[v]) + " " + to_string(obj_numerator));

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

            value[u] -= deriv_deg[v] * deg_loc_norm_pow(e.layer, q_rec);

            auto deg_norms = deg_norm_pow(deg[u], q, p_alpha, alpha);
            auto deriv_deg_u = deg_norms.first * p;
            auto agg_deg_u = deg_norms.second;
            auto delta = agg_deg[u] - agg_deg_u;
            value[u] -= delta;
            obj_numerator -= delta;

            auto deg_norm = deg_norm_pow(deg[u], q, q_rec);
            if (deg_norm_cpy[u] > upd_factor * deg_norm) {
                for (auto &e2: nbrs[u]) {
                    auto w = e2.nbr;
                    if (!exists[w]) continue;

                    if (!aff[w]) {
                        heap.Remove(w);
                        affected_nbr.emplace_back(w);
                        aff[w] = true;
                    }

                    value[w] -= (deriv_deg[u] - deriv_deg_u) * deg_loc_norm_pow(e2.layer, q_rec);
                }
                deg_norm_cpy[u] = deg_norm;
            }

            agg_deg[u] = agg_deg_u;
            deriv_deg[u] = deriv_deg_u;
        }

        for (auto u: affected_nbr) {
            heap.Insert(u);
            aff[u] = false;
        }

        /** compute the object func **/
        obj = double(obj_numerator / (n - i - 1));
        if (obj > obj_max) {
            obj_max = obj;
            obj_max_offset = i + 1;
        }
    }

    del_seq[n - 1] = heap.RemoveMin();
    gds.assign(del_seq.begin() + obj_max_offset, del_seq.end());
    return pow(obj_max * pow(1.0 / ln, alpha), 1 / p);
}

template<class HEAP>
        double Greedy_p1q(MLG &mlg, double q, double p, vector<uint> &gds) {

    double p_rec = 1 / p, q_rec = 1 / q, alpha = p * q_rec, obj, obj_max;
    l_double obj_numerator = 0;
    uint n = mlg.GetN(), ln = mlg.GetLn(), obj_max_offset;

    vector<vector<uint>> deg;
    vector<uint> del_seq(n), deg_sum(n);
    vector<bool> exists(n, true);
    vector<double> agg_deg(n);
    HEAP heap(n);

    /** compute aggregated deg and initiate score **/
    mlg.GetDegree(deg);
    auto &value = heap.GetValue();
    for (uint v = 0; v < n; v++) {
        auto deg_norms = deg_norm_pow(deg[v], q, q_rec, alpha);
        agg_deg[v] = deg_norms.second;
        obj_numerator += agg_deg[v];

        value[v] = deg_norms.first;
        deg_sum[v] = sum(deg[v]);
    }
    heap.InsertAll();

    obj_max = pow(double(obj_numerator / n), p_rec);
    obj_max_offset = 0;

    uint i = 0;
    auto &nbrs = mlg.GetNbrs();
    while (i < n - 1) {

        // select v with min score
        auto v = heap.RemoveMin();

        del_seq[i++] = v;
        exists[v] = false;
        obj_numerator -= agg_deg[v];

        print_debug_info(std::to_string(i) + " " + std::to_string(v) + " " + std::to_string(value[v]) + " " + std::to_string(obj_numerator) + " " +
        std::to_string(pow(double(obj_numerator / (n - i - 1)), p_rec)));

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
                obj_numerator -= agg_deg[u];
            } else {

                heap.Remove(u);

                auto deg_norms = deg_norm_pow(deg[u], q, q_rec, alpha);
                auto score_u = deg_norms.first;
                auto agg_deg_u = deg_norms.second;
                obj_numerator -= agg_deg[u] - agg_deg_u;

                agg_deg[u] = agg_deg_u;
                value[u] = score_u;
                heap.Insert(u);
            }
        }

        /** compute the object func **/
        if (i < n) {
            obj = pow(double(obj_numerator / (n - i)), p_rec);
            if (obj > obj_max) {
                obj_max = obj;
                obj_max_offset = i;
            }
        }
    }

    gds.assign(del_seq.begin() + obj_max_offset, del_seq.end());
    return obj_max * pow(1.0 / ln, q_rec);
}

template<class HEAP>
        double Greedy_min1q(MLG &mlg, double q, vector<uint> &gds) {

    double q_rec = 1 / q, obj_max;
    uint n = mlg.GetN(), ln = mlg.GetLn(), obj_max_offset;

    vector<vector<uint>> deg;
    vector<uint> del_seq(n);
    vector<bool> exists(n, true);
    HEAP heap(n);

    auto deg_norm_pow_refine = q > 0 ? deg_norm_pow_posp: deg_norm_pow_negq;

    /** compute initiate score **/
    mlg.GetDegree(deg);
    auto &value = heap.GetValue();
    for (uint v = 0; v < n; v++) {
        value[v] = deg_norm_pow_refine(deg[v], q, q_rec);
    }
    heap.InsertAll();

    obj_max = 0;
    obj_max_offset = -1;

    auto &nbrs = mlg.GetNbrs();
    for (uint i = 0; i < n; i++) {
        // select v with min score
        auto v = heap.RemoveMin();

        del_seq[i] = v;
        if (obj_max_offset == -1 || value[v] > obj_max) {
            obj_max = value[v];
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

            heap.Update(u, deg_norm_pow_refine(deg[u], q, q_rec));
        }
    }

    gds.assign(del_seq.begin() + obj_max_offset, del_seq.end());
    return obj_max * pow(1.0 / ln, q_rec);
}

double Greedy_min1(MLG &mlg, vector<uint> &gds) {

    uint n = mlg.GetN(), max_min_deg, obj_max_offset, obj_max;

    vector<vector<uint>> deg;
    vector<uint> agg_deg(n), del_seq(n);
    vector<bool> exists(n, true);

    /** build initial int-heap and agg_deg **/
    mlg.GetDegree(deg);
    max_min_deg = 0;
    for (uint v = 0; v < n; v++) {
        auto d_min = min(deg[v]);
        if (d_min > max_min_deg) {
            max_min_deg = d_min;
        }
        agg_deg[v] = d_min;
    }

    IntHeap ilh(n, max_min_deg);
    for (uint v = 0; v < n; v++) {
        ilh.Insert(v, agg_deg[v]);
    }

    obj_max = 0;
    obj_max_offset = -1;

    auto &nbrs = mlg.GetNbrs();
    for (uint i = 0; i < n; i++) {

        // select v with min score
        auto v = ilh.SelectMin();

        del_seq[i] = v;
        if (obj_max_offset == -1 || agg_deg[v] > obj_max) {
            obj_max = agg_deg[v];
            obj_max_offset = i;
        }

        exists[v] = false;
        ilh.Remove(v, agg_deg[v]);

        // update score of v's 1-hop neighbors
        for (auto &e: nbrs[v]) {
            auto u = e.nbr;
            if (!exists[u]) continue;

            for (auto l: e.layer) {
                deg[u][l]--;
            }

            ilh.Remove(u, agg_deg[u]);
            agg_deg[u] = min(deg[u]);
            ilh.Insert(u, agg_deg[u]);
        }
    }

    gds.assign(del_seq.begin() + obj_max_offset, del_seq.end());
    return obj_max;
}
