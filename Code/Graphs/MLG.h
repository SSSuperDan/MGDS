#ifndef MLGDENSEST_MLG_H
#define MLGDENSEST_MLG_H

#include "../Header.h"
#include "../Utils/ArrOps.h"

#define DEFAULT_VTX_SIZE 5000

struct edge{
    uint v{0};
    uint l{0};
    edge() = default;
    edge(uint v_, uint l_): v(v_), l(l_){};
    bool operator < (const edge & e) const {
        return v < e.v || (v == e.v && l < e.l);
    }
};

struct ml_edge{
    uint nbr{};
    vector<uint> layer;
    ml_edge() = default;
    explicit ml_edge(uint nbr_): nbr(nbr_){};
};

class MLG {
public:
    MLG();
    ~MLG() = default;

    void Load(const string &i_path);
    void PrtStats();

    void Map2Vtx(vector<uint>& ids, vector<ll_uint>& o_vtx);
    void Map2VtxId(vector<ll_uint>& o_vtx, vector<uint>& ids);
    static MLG *LoadMLG(const string &file);

    [[nodiscard]] inline uint GetLn() const {
        return ln;
    }

    [[nodiscard]] inline uint GetN() const {
        return n;
    }

    [[nodiscard]] inline vector<vector<ml_edge>>& GetNbrs() {
        return nbrs;
    }

    void GetDegree(vector<vector<uint>> &deg);
    uint GetM();
    uint GetMaxDeg();

private:
    vector<vector<ml_edge>> nbrs;

    uint ln{0};
    uint n{0};

    string map_file;

    void LoadId2VtxMap(vector<ll_uint> &id2vtx);
    void LoadVtx2IdMap(unordered_map<ll_uint, uint> &vtx2id);

    void LoadLayer(const string &g_file, uint l, unordered_map<ll_uint, uint> &vtx2id, vector<vector<edge>>& edges,
                   basic_ofstream<char> &map_out);
    void LoadLayer(const string &g_file, unordered_map<ll_uint, uint> &vtx2id, vector<vector<edge>>& edges,
                   basic_ofstream<char> &map_out);
    void BuildNbrs(vector<vector<edge>>& edges);
    static void GetLayerFile(const string &i_path, vector<string> &g_files);

};


#endif //MLGDENSEST_MLG_H
