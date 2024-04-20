#ifndef MLGDENSEST_STATS_H
#define MLGDENSEST_STATS_H


#include "../Utils/ArrOps.h"
#include "../Utils/StrOps.h"
#include "GFirmCore.h"
#include "GDS.h"

void GetPartDegs(MLG &mlg, const vector<uint> &vtx, vector<vector<uint>> &degs);

void GetNEdges(uint ln, const vector<vector<uint>> &degs, vector<uint> &n_edges);

void GetPerLayerEdgeDensity(uint ln, uint n, const vector<uint> &n_edges, vector<double> &layer_density);

double GetEdgeDensity(uint ln, uint n, const vector<uint> &n_edges);

void GetDegPower(uint ln, const vector<vector<uint>> &degs, vector<l_double> &n_edge_power, double p);

void GetPerLayerDensity(uint ln, uint n, const vector<l_double> &n_edge_power, vector<double> &layer_density, double p);

void GetPerLayerDensityMinP(uint ln, const vector<vector<uint>> &degs, vector<double> &layer_density);

double GetLayerDensity(uint n, vector<l_double> &n_edge_power, double p, double beta);

double GetLayerDensityMinP(uint ln, vector<vector<uint>> &degs, double beta);

double GetGDensity(vector<vector<uint>> &degs, double q, double p);

double GetGDensityMinP(vector<vector<uint>> &degs, double &q);

double GetGDensityMinPQ(vector<vector<uint>> &degs);


/*
 * Compute the following statistics about the given vertex subset:
 *
 * per_layer_edge_num, per_layer_edge_density, per_layer_density
 * edge_density, layer_density, greedy_density
 *
*/
template<class F>
void GetStats(MLG &mlg, const vector<uint> &vtx, F &output, const string &q, const string &p, double beta) {
    uint ln = mlg.GetLn(), n = vtx.size();
    vector<uint> n_edges;
    vector<vector<uint>> degs;
    vector<l_double> n_edge_power;
    vector<double> per_layer_edge_density, per_layer_density;
    double edge_density, layer_density, greedy_density;

    GetPartDegs(mlg, vtx, degs);
    GetNEdges(ln, degs, n_edges);
    GetPerLayerEdgeDensity(ln, n, n_edges, per_layer_edge_density);
    edge_density = GetEdgeDensity(ln, n, n_edges);

    if (IsDigit(p)) {
        auto p_value = stod(p);

        GetDegPower(ln, degs, n_edge_power, p_value);
        GetPerLayerDensity(ln, n, n_edge_power, per_layer_density, p_value);
        layer_density = GetLayerDensity(n, n_edge_power, p_value, beta);

        assert(IsDigit(q));
        auto q_value = stod(q);
        greedy_density = GetGDensity(degs, q_value, p_value);

    } else {
        assert (p == "-inf");
        GetPerLayerDensityMinP(ln, degs, per_layer_density);
        layer_density = GetLayerDensityMinP(ln, degs, beta);

        if (IsDigit(q)) {
            auto q_value = stod(q);
            greedy_density = GetGDensityMinP(degs, q_value);
        } else {
            assert (q == "-inf");
            greedy_density = GetGDensityMinPQ(degs);
        }
    }

    output << "size = " << vtx.size() << endl;
    output << "per_layer_edge_num = " << Vec2Str(n_edges) << endl;
    output << "per_layer_edge_density = " << Vec2Str(per_layer_edge_density) << endl;
    output << "per_layer_density = " << Vec2Str(per_layer_density) << endl;
    output << "edge_density = " << edge_density << endl;
    output << "layer_density = " << layer_density << endl;
    output << "greedy_density = " << greedy_density << endl;
}


/*
 * Compute the following statistics about the given vertex subset:
 *
 * per_layer_edge_num, per_layer_edge_density, per_layer_density
 * edge_density, layer_density, greedy_density
 *
 * for varying q, p, beta
 *
*/
template<class F>
void GetStats(MLG &mlg, const vector<uint> &vtx, F &output) {
    uint ln = mlg.GetLn(), n = vtx.size();
    vector<uint> n_edges;
    vector<vector<uint>> degs;
    vector<l_double> n_edge_power;
    vector<double> per_layer_edge_density, per_layer_density;
    double edge_density, layer_density, greedy_density;

    vector<double> default_betas = {1, 1.5, 2};
    vector<double> default_ps = {-1, -0.5, 0.5, 1, 1.5, 2};

    /* number of edges and edge densities*/
    GetPartDegs(mlg, vtx, degs);
    GetNEdges(ln, degs, n_edges); // number of edges
    GetPerLayerEdgeDensity(ln, n, n_edges, per_layer_edge_density); // per-layer edge density
    edge_density = GetEdgeDensity(ln, n, n_edges); // overall edge_density

    output << "size = " << vtx.size() << endl;
    output << "per_layer_edge_num = " << Vec2Str(n_edges) << endl;
    output << "per_layer_edge_density = " << Vec2Str(per_layer_edge_density) << endl;
    output << "edge_density = " << edge_density << endl;

    /*layer density*/
    for (auto &p:default_ps) {
        GetDegPower(ln, degs, n_edge_power, p);
        GetPerLayerDensity(ln, n, n_edge_power, per_layer_density, p);
        output << "per_layer_density (p = " << to_string(p) << ") = " << Vec2Str(per_layer_density) << endl;

        for (auto &beta:default_betas) {
            layer_density = GetLayerDensity(n, n_edge_power, p, beta);
            output << "layer_density (p = " << to_string(p) << ", b = " << to_string(beta) << ") = " << layer_density
                   << endl;
        }
        n_edge_power.clear();
        per_layer_density.clear();
    }
    GetPerLayerDensityMinP(ln, degs, per_layer_density);
    output << "per_layer_density (p = -inf) = " << Vec2Str(per_layer_density) << endl;

    for (auto &beta:default_betas) {
        layer_density = GetLayerDensityMinP(ln, degs, beta);
        output << "layer_density (p = -inf, b = " << to_string(beta) << ") = " << layer_density << endl;
    }

    /*GDS density*/

    // p = -inf
    greedy_density = GetGDensityMinPQ(degs);
    output << "greedy_density (p = -inf, q = -inf) = " << greedy_density << endl;

    for (auto q:{-1.0, -0.5, 0.5, 1.0, 1.5, 2.0}) {
        greedy_density = GetGDensityMinP(degs, q);
        output << "greedy_density (p = -inf, q = " << to_string(q) << ") = " << greedy_density << endl;
    }

    // p = 1
    double p = 1;
    for (auto q:{1.0, 1.25, 1.5, 1.75, 2.0}) {
        greedy_density = GetGDensity(degs, q, p);
        output << "greedy_density (p = 1, q = " << to_string(q) << ") = " << greedy_density << endl;
    }

    // q = 1
    double q = 1;
    for (auto p:{-1.0, -0.5, 0.5, 1.5, 2.0}) {
        greedy_density = GetGDensity(degs, q, p);
        output << "greedy_density (p = " << to_string(p) << ", q = 1) = " << greedy_density << endl;
    }
}


/*
 * Compute the following statistics about the given vertex subset:
 *
 * per_layer_edge_num, per_layer_edge_density, per_layer_density
 * edge_density, layer_density, greedy_density
 *
 * for varying q, p, beta
 *
*/
template<class F>
void GetNodeStats(MLG &mlg, const vector<uint> &vtx, F &output) {
    vector<vector<uint>> degs;

    /* number of edges and edge densities*/
    GetPartDegs(mlg, vtx, degs);
    for (auto &deg : degs) {
        output << Vec2Str(deg) << endl;
    }
}

#endif //MLGDENSEST_STATS_H
