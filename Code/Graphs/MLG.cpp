#include "MLG.h"

MLG::MLG() {
    nbrs.reserve(DEFAULT_VTX_SIZE);
}

void MLG::Load(const string &i_path) {
    unordered_map<ll_uint, uint> vtx2id;
    vector<vector<edge>> edges;

    map_file = i_path + "__vtx_map.txt";
    auto map_out = ofstream(map_file);

    if (is_directory(i_path)) {
        /* given a directory containing graph files*/

        vector<string> g_files;
        GetLayerFile(i_path, g_files);
        ln = g_files.size();

        for (uint i = 0; i < ln; i++) {
            LoadLayer(i_path + g_files[i], i, vtx2id, edges, map_out);
        }

    } else if (is_regular_file(i_path)) {
        /* given a single graph file*/
        LoadLayer(i_path, vtx2id, edges, map_out);
    }

    map_out.close();
    BuildNbrs(edges);
}

void MLG::PrtStats() {
    if (!n) {
        print("Empty Graph.");
        return;
    }

    uint m[ln], m_max = 0, m_min = INT32_MAX, m_total = 0;
    memset(m, 0, ln * sizeof(uint));

    for (uint i = 0; i < n; i++) {
        for (auto &e: nbrs[i]) {
            for (auto l: e.layer) {
                m[l] += 1;
            }
        }
    }

    for (uint i = 0; i < ln; i++) {
        m[i] = m[i] >> 1;
        if (m[i] > m_max) m_max = m[i];
        if (m[i] < m_min) m_min = m[i];
        m_total += m[i];
    }

    print("===== Graph info ===== ");
    print(" |L| = " + to_string(ln));
    print(" m = " + Arr2Str(m, ln));
    print(" m_min = " + to_string(m_min));
    print(" m_max = " + to_string(m_max));
    print(" m_avg = " + to_string(m_total / ln));
    print(" m_total = " + to_string(m_total));
    print(" n_vert = " + to_string(n));
    print("====================== ");

}

void MLG::Map2Vtx(vector<uint> &ids, vector<ll_uint> &o_vtx) {

    vector<ll_uint> id2vtx;
    LoadId2VtxMap(id2vtx);

    o_vtx.reserve(ids.size());
    for (auto v: ids) {
        o_vtx.emplace_back(id2vtx[v]);
    }
}

void MLG::Map2VtxId(vector<ll_uint> &o_vtx, vector<uint> &ids) {

    unordered_map<ll_uint, uint> vtx2id;
    LoadVtx2IdMap(vtx2id);

    ids.reserve(o_vtx.size());
    for (auto v:o_vtx) {
        ids.emplace_back(vtx2id[v]);
    }
}


void MLG::LoadId2VtxMap(vector<ll_uint> &id2vtx) {
    ll_uint u;
    uint uid;

    id2vtx.resize(n);
    ifstream fin(map_file);
    while (fin.good() && !fin.eof()) {
        fin >> u >> uid;
        id2vtx[uid] = u;
    }
    fin.close();
}

void MLG::LoadVtx2IdMap(unordered_map<ll_uint, uint> &vtx2id) {
    ll_uint u;
    uint uid;

    ifstream fin(map_file);
    while (fin.good() && !fin.eof()) {
        fin >> u >> uid;
        vtx2id[u] = uid;
    }
    fin.close();
}

MLG *MLG::LoadMLG(const string &file) {
    auto mg = new MLG();
    mg->Load(file);
    return mg;
}

void MLG::GetDegree(vector<vector<uint>> &deg) {
    deg.resize(n);

    for (uint v = 0; v < n; v++) {
        deg[v].resize(ln, 0);
        for (auto &e: nbrs[v]) {
            for (auto l: e.layer) {
                deg[v][l] += 1;
            }
        }
    }
}

uint MLG::GetM() {
    uint m = 0;
    for (uint v = 0; v < n; v++) {
        for (auto &e: nbrs[v]) {
            m += e.layer.size();
        }
    }

    return (m >> 1);
}

uint MLG::GetMaxDeg() {
    uint max_deg = 0, d;
    for (uint v = 0; v < n; v++) {
        d = nbrs[v].size();
        max_deg = std::max(d, max_deg);
    }

    return max_deg;
}

void
MLG::LoadLayer(const string &g_file, uint l, unordered_map<ll_uint, uint> &vtx2id, vector<vector<edge>> &edges,
               basic_ofstream<char> &map_out) {
    uint uid, vid;
    ll_uint u, v;

    auto graph_in = ifstream(g_file);

    while (graph_in.peek() != EOF) {
        graph_in >> u >> v;

        // remove self-loop
        if (u != v) {

            auto iter1 = vtx2id.find(u);
            if (iter1 != vtx2id.end()) {
                uid = iter1->second;
            } else {
                uid = n++;
                vtx2id.emplace(u, uid);
                edges.emplace_back();
                map_out << u << " " << uid << endl;
            }

            auto iter2 = vtx2id.find(v);
            if (iter2 != vtx2id.end()) {
                vid = iter2->second;
            } else {
                vid = n++;
                vtx2id.emplace(v, vid);
                edges.emplace_back();
                map_out << v << " " << vid << endl;
            }

            edges[uid].emplace_back(vid, l);
            edges[vid].emplace_back(uid, l);
        }
    }
    graph_in.close();
}

void
MLG::LoadLayer(const string &g_file, unordered_map<ll_uint, uint> &vtx2id, vector<vector<edge>> &edges,
               basic_ofstream<char> &map_out) {
    uint uid, vid, lid;
    ll_uint u, v, l;
    unordered_map<uint, uint> layer2id;

    auto graph_in = ifstream(g_file);

    while (graph_in.peek() != EOF) {
        graph_in >> u >> v >> l;

        // remove self-loop
        if (u != v) {

            auto iter1 = vtx2id.find(u);
            if (iter1 != vtx2id.end()) {
                uid = iter1->second;
            } else {
                uid = n++;
                vtx2id.emplace(u, uid);
                edges.emplace_back();
                map_out << u << " " << uid << endl;
            }

            auto iter2 = vtx2id.find(v);
            if (iter2 != vtx2id.end()) {
                vid = iter2->second;
            } else {
                vid = n++;
                vtx2id.emplace(v, vid);
                edges.emplace_back();
                map_out << v << " " << vid << endl;
            }

            auto iter3 = layer2id.find(l);
            if (iter3 != layer2id.end()) {
                lid = iter3->second;
            } else {
                lid = layer2id.size();
                layer2id.emplace(l, lid);
            }

            edges[uid].emplace_back(vid, lid);
            edges[vid].emplace_back(uid, lid);
        }
    }
    graph_in.close();
    ln = layer2id.size();
}

void MLG::BuildNbrs(vector<vector<edge>> &edges) {
    uint n_nbr, last_v, last_l;
    ml_edge *back_ptr;

    nbrs.resize(n);

    for (uint i = 0; i < n; i++) {
        sort(edges[i].begin(), edges[i].end());
        n_nbr = edges[i].size();
        last_v = last_l = -1;

        for (uint j = 0; j < n_nbr; j++) {
            if (edges[i][j].v != last_v) {
                last_v = edges[i][j].v;
                last_l = -1;
                nbrs[i].emplace_back(last_v);
                back_ptr = &nbrs[i].back();
            }

            if (edges[i][j].l != last_l) {
                back_ptr->layer.emplace_back(edges[i][j].l);
                last_l = edges[i][j].l;
            }
        }
    }
}

void MLG::GetLayerFile(const string &i_path, vector<string> &g_files) {
    struct stat buffer{};
    string &&conf_file = i_path + "mlg.conf";

    if (stat((conf_file).c_str(), &buffer)) {
        // When no configure file is provided, all files in the folder are included.

        DIR *dir;
        dirent *env;
        if ((dir = opendir(i_path.c_str()))) {
            while ((env = readdir(dir))) {
                if (env->d_name[0] != '_' and env->d_name[0] != '.') {
                    g_files.emplace_back(env->d_name);
                }
            }
            closedir(dir);
        }

    } else {
        string line;

        auto fin = ifstream(conf_file);
        while (fin.peek() != EOF) {

            getline(fin, line);

            if (!line.empty()) {
                g_files.emplace_back(line);
            }
        }
        fin.close();
    }
}
