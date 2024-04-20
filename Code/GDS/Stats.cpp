#include "Stats.h"

void GetPartDegs(MLG &mlg, const vector<uint> &vtx, vector<vector<uint>> &degs) {
    uint n = mlg.GetN(), ln = mlg.GetLn();
    vector<bool> sign(n, false);

    for (auto v:vtx) {
        sign[v] = true;
    }

    degs.reserve(vtx.size());
    auto &nbrs = mlg.GetNbrs();
    for (auto v:vtx) {
        vector<uint> deg(ln, 0);

        for (auto &e:nbrs[v]) {
            auto u = e.nbr;
            if (sign[u]) {
                for (auto l:e.layer) {
                    deg[l] += 1;
                }
            }
        }

        /* check no isolate vertex */
        bool all_zero = true;
        for (uint l = 0; l < ln; l++) {
            if (deg[l]) {
                all_zero = false;
                break;
            }
        }

        if (all_zero) {
            print("Error!");
        }

        degs.emplace_back(deg);
    }
}

void GetNEdges(uint ln, const vector<vector<uint>> &degs, vector<uint> &n_edges) {
    n_edges.resize(ln, 0);

    for (const auto &deg : degs) {
        for (uint l = 0; l < ln; l++) {
            n_edges[l] += deg[l];
        }
    }

    for (uint l = 0; l < ln; l++) {
        n_edges[l] = n_edges[l] >> 1;
    }
}

void GetPerLayerEdgeDensity(uint ln, uint n, const vector<uint> &n_edges, vector<double> &layer_density) {
    layer_density.resize(ln, 0);

    for (uint l = 0; l < ln; l++) {
        layer_density[l] = double(2 * n_edges[l]) / (n * (n - 1));
    }
}

double GetEdgeDensity(uint ln, uint n, const vector<uint> &n_edges) {
    uint tot_edge_num = 0;

    for (uint l = 0; l < ln; l++) tot_edge_num += n_edges[l];
    return double(2 * tot_edge_num) / (n * (n - 1));
}

void GetDegPower(uint ln, const vector<vector<uint>> &degs, vector<l_double> &n_edge_power, double p) {
    n_edge_power.resize(ln, 0);

    for (const auto &deg : degs) {
        for (uint l = 0; l < ln; l++) {
            n_edge_power[l] += (deg[l] > 0 || p > 0) ? pow(deg[l], p) : pow(SMALL_VALUE, p);
        }
    }
}

void GetPerLayerDensity(uint ln, uint n, const vector<l_double> &n_edge_power, vector<double> &layer_density, double p) {
    double p_rec = 1 / p;
    layer_density.resize(ln);

    for (uint l = 0; l < ln; l++) {
        layer_density[l] = pow((double) (n_edge_power[l] / n), p_rec);
    }
}

void GetPerLayerDensityMinP(uint ln, const vector<vector<uint>> &degs, vector<double> &layer_density) {
    layer_density.resize(ln, UINT_MAX);

    for (const auto &deg : degs) {
        for (uint l = 0; l < ln; l++) {
            layer_density[l] = std::min(layer_density[l], double(deg[l]));
        }
    }
}

double GetLayerDensity(uint n, vector<l_double> &n_edge_power, double p, double beta) {
    return GetGDen(n_edge_power, n, p, beta);
}

double GetLayerDensityMinP(uint ln, vector<vector<uint>> &degs, double beta) {
    double max_den = 0, den;
    vector<uint> den_vec(ln, UINT_MAX);

    for (const auto &deg : degs) {
        for (uint l = 0; l < ln; l++) {
            den_vec[l] = std::min(den_vec[l], deg[l]);
        }
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

double GetGDensity(vector<vector<uint>> &degs, double q, double p) {
    uint ln = degs[0].size(), n = degs.size();
    l_double numerator = 0;
    double alpha = p / q, layer_factor = pow(1.0 / ln, alpha);

    auto deg_norm_pow_refine = q > 0 ? deg_norm_pow_posp: deg_norm_pow_negq;

    for (auto &deg : degs) {
        numerator += deg_norm_pow_refine(deg, q, alpha) * layer_factor;
    }
    return pow(double(numerator / n), 1 / p);
}

double GetGDensityMinP(vector<vector<uint>> &degs, double &q) {
    uint ln = degs[0].size();
    double overall_min = DBL_MAX, q_rec = 1 / q, layer_factor = pow(1.0 / ln, q_rec);

    auto deg_norm_pow_refine = q > 0 ? deg_norm_pow_posp: deg_norm_pow_negq;

    for (auto &deg : degs) {
        auto d = deg_norm_pow_refine(deg, q, q_rec) * layer_factor;
        if (d < overall_min) {
            overall_min = d;
        }
    }
    return overall_min;
}

double GetGDensityMinPQ(vector<vector<uint>> &degs) {
    uint overall_min = UINT_MAX;

    for (auto &deg : degs) {
        auto d = min(deg);
        if (d < overall_min) {
            overall_min = d;
        }
    }
    return overall_min;
}
