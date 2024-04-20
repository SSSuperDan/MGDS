#include "GFirmCore.h"

void UpdateDensestFC(DFC_INFO &densest_fc, vector<uint> &coreness, vector<uint> &dsg) {
    uint n = coreness.size();
    dsg.clear();
    dsg.reserve(densest_fc.size);
    for (uint v = 0; v < n; v++) {
        if (coreness[v] >= densest_fc.k) {
            dsg.emplace_back(v);
        }
    }
    assert(dsg.size() == densest_fc.size);
}

DFC_INFO DensestFCLamb(MLG &mlg, vector<uint> &coreness, uint lamb, double beta) {
    uint ln = mlg.GetLn(), n = mlg.GetN(), k = 0, maxK = 0, n_vtx = n;
    vector<uint> n_edges(ln, 0);
    vector<vector<uint>> deg;
    DFC_INFO densest_fc;
    double den;

    /* Init */
    mlg.GetDegree(deg);
    for (uint v = 0; v < n; v++) {
        for (uint l = 0; l < ln; l++) {
            n_edges[l] += deg[v][l];
        }
    }

    for (uint v = 0; v < n; v++) {
        coreness[v] = findTopK(deg[v].data(), ln, lamb);
        maxK = max(maxK, coreness[v]);
    }

    IntHeap heap(n, maxK);
    for (uint v = 0; v < n; v++) {
        heap.Insert(v, coreness[v]);
    }

    /* Peeling */
    auto &nbrs = mlg.GetNbrs();
    while (n_vtx) {

        vector<uint> to_remove;
        set<uint> to_check;
        auto new_k = heap.GetMin(to_remove);

        /* compute the density of the current firmcore (including v) */
        if (new_k > k) {
            k = new_k;
            den = GetDen(n_edges, n_vtx, beta);
            if (den > densest_fc.den) {
                densest_fc.Update(k, n_vtx, den);
            }
        }

        n_vtx -= to_remove.size();
        for (auto v: to_remove) {
            for (uint l = 0; l < ln; l++) {
                n_edges[l] -= deg[v][l];
            }

            /* refine neighbors of v */
            for (auto &e: nbrs[v]) {
                auto u = e.nbr;

                if (coreness[u] > k) {
                    for (auto l: e.layer) {
                        if (deg[u][l] == coreness[u]) {
                            to_check.insert(u);
                        }
                        deg[u][l] -= 1;
                        n_edges[l] -= 1;
                    }
                }
            }

            for (auto u: to_check) {
                auto new_topK = findTopK(deg[u].data(), ln, lamb);
                if (new_topK != coreness[u] and new_topK <= k) {
                    heap.Update(u, coreness[u], k);
                    coreness[u] = k;
                } else if (new_topK != coreness[u]) {
                    heap.Update(u, coreness[u], new_topK);
                    coreness[u] = new_topK;
                } else {
                    continue;
                }
            }
            to_check.clear();
        }
    }
    return densest_fc;
}

double GetDen(vector<uint> &n_edges, uint n_vtx, double beta) {
    uint ln = n_edges.size();
    double max_den = 0, den;
    vector<double> den_vec(ln);

    for (uint l = 0; l < ln; l++) {
        den_vec[l] = (double(n_edges[l] >> 1)) / n_vtx;
    }

    std::sort(den_vec.begin(), den_vec.end());
    for (int i = (int) ln - 1; i >= 0; i--) {
        den = den_vec[i] * pow(ln - i, beta);
        if (den > max_den) {
            max_den = den;
        }
    }
    return max_den;
}

double DensestFC(MLG &mlg, vector<uint> &dsg, double beta) {
    uint ln = mlg.GetLn(), n = mlg.GetN();
    vector<uint> coreness(n);
    double max_den = 0;

    for (uint lamb = 1; lamb <= ln; lamb++) {
        auto densest_fc = DensestFCLamb(mlg, coreness, lamb, beta);
        if (densest_fc.den > max_den) {
            UpdateDensestFC(densest_fc, coreness, dsg);
            max_den = densest_fc.den;
        }
    }
    return max_den;
}

template<class HEAP>
        DFC_INFO DensestGFCLamb(MLG &mlg, vector<uint> &coreness, uint lamb, double p, double beta) {
    uint ln = mlg.GetLn(), n = mlg.GetN(), k = 0, n_vtx = n;
    vector<uint> score_floor(ln), deg_pre(ln);
    vector<vector<uint>> deg;
    vector<vector<double>> score, deg_power;
    vector<l_double> n_edge_power(ln, 0);
    vector<double> deg_power_pre(ln);
    vector<bool> exist(n, true), affected(n, false);
    DFC_INFO densest_gfc;
    MixHeap heap(n);
    double den;

    /* Init */
    mlg.GetDegree(deg);
    score.resize(n);
    deg_power.resize(n);

    for (uint v = 0; v < n; v++) {
        score[v].resize(ln);
        deg_power[v].resize(ln);

        for (uint l = 0; l < ln; l++) {
            deg_power[v][l] = pow(deg[v][l], p);
            score[v][l] = deg_power[v][l];
            n_edge_power[l] += score[v][l];
        }
    }

    auto &value = heap.GetValue();
    auto &nbrs = mlg.GetNbrs();
    for (uint v = 0; v < n; v++) {
        for (auto &e: nbrs[v]) {
            auto u = e.nbr;
            for (auto l: e.layer) {
                score[v][l] += deg_power[u][l] - pow(deg[u][l] - 1, p);
            }
        }

        for (uint l = 0; l < ln; l++) {
            score_floor[l] = uint(score[v][l]);
        }
        value[v] = findTopK(score_floor.data(), ln, lamb);
    }
    heap.InsertAll();

    /* Peeling */
    for (uint i = 0; i < n - 1; i++) {
        vector<uint> to_check;

        auto v = heap.RemoveMin();
        auto new_k = (uint) value[v];

        /* compute the density of the current fc */
        if (new_k > k) {
            k = new_k;
            den = GetGDen(n_edge_power, n_vtx, p, beta);
            if (den > densest_gfc.den) {
                densest_gfc.Update(k, n_vtx, den);
            }
        }

        exist[v] = false;
        coreness[v] = k;
        n_vtx -= 1;
        for (uint l = 0; l < ln; l++) {
            n_edge_power[l] -= deg_power[v][l];
        }

        /* refine 2-hop neighbors of v */
        for (auto &e: nbrs[v]) {
            auto u = e.nbr;
            if (!exist[u]) continue;

            deg_pre.assign(deg[u].begin(), deg[u].end());
            deg_power_pre.assign(deg_power[u].begin(), deg_power[u].end());

            for (auto l: e.layer) {
                auto old_deg_power = deg_power[u][l];
                auto new_deg_power = pow(deg[u][l] - 1, p);
                auto delta = old_deg_power - new_deg_power;

                deg_power[u][l] = new_deg_power;
                n_edge_power[l] -= delta;

                if (uint(score[u][l]) == value[u] and !affected[u]) {
                    to_check.emplace_back(u);
                    affected[u] = true;
                }

                score[u][l] -= delta;
                score[u][l] -= deg_power[v][l] - pow(deg[v][l] - 1, p);

                deg[u][l]--;
            }

            for (auto &e2: nbrs[u]) {
                auto w = e2.nbr;
                if (!exist[w]) continue;

                for (auto l: e2.layer) {

                    if (uint(score[w][l]) == value[w] and !affected[w]) {
                        to_check.emplace_back(w);
                        affected[w] = true;
                    }

                    score[w][l] -= (deg_power_pre[l] - pow(deg_pre[l] - 1, p)) -
                            (deg_power[u][l] - pow(deg[u][l] - 1, p));
                }
            }
        }

        for (auto u: to_check) {
            for (uint l = 0; l < ln; l++) {
                score_floor[l] = uint(score[u][l]);
            }

            auto new_value = findTopK(score_floor.data(), ln, lamb);
            heap.Update(u, new_value);
            affected[u] = false;
        }
    }

    auto v = heap.RemoveMin();
    coreness[v] = k;

    return densest_gfc;
}

double GetGDen(vector<l_double> &n_edge_power, uint n_vtx, double p, double beta) {
    uint ln = n_edge_power.size();
    double p_rec = 1 / p, max_den = 0, den;
    vector<double> den_vec(ln);

    for (uint l = 0; l < ln; l++) {
        den_vec[l] = pow((double) (n_edge_power[l] / n_vtx), p_rec);
    }

    std::sort(den_vec.begin(), den_vec.end());
    for (int i = (int) ln - 1; i >= 0; i--) {
        den = den_vec[i] * pow(ln - i, beta);
        if (den > max_den) {
            max_den = den;
        }
    }
    return max_den;
}

double DensestGFC(MLG &mlg, vector<uint> &dsg, const string &p, double beta, bool use_heap) {
    uint ln = mlg.GetLn(), n = mlg.GetN();
    vector<uint> coreness(n);
    double max_den = 0;

    if (IsDigit(p)) {
        auto p_value = stod(p);
        auto DFCLamb = use_heap ? DensestGFCLamb<MixHeap> : DensestGFCLamb<SetHeap>;
        for (uint lamb = 1; lamb <= ln; lamb++) {
            auto densest_fc = DFCLamb(mlg, coreness, lamb, p_value, beta);
            if (densest_fc.den > max_den) {
                UpdateDensestFC(densest_fc, coreness, dsg);
                max_den = densest_fc.den;
            }
        }
    } else {
        assert(p == "-inf");

        // use firmcore to approximate (p = 1)
        for (uint lamb = 1; lamb <= ln; lamb++) {
            auto densest_fc = DensestFCLamb(mlg, coreness, lamb, beta); // use the case for p = 1 to approximate
            if (densest_fc.den > max_den) {
                UpdateDensestFC(densest_fc, coreness, dsg);
                max_den = densest_fc.den;
            }
        }
    }
    return max_den;
}
