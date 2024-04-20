#include "Utils/Timer.h"
#include "GDS/GDS.h"
#include "GDS/Stats.h"
#include "GDS/GFirmCore.h"
#include "GDS/GDSMultiPrecision.h"

DEFINE_string(gpath, "../datasets/homo/", "graph path");
DEFINE_string(gname, "homo", "graph name");
DEFINE_string(mode, "", "running a specific greedy algorithm");
DEFINE_string(tf, "", "tested ds files");
DEFINE_string(o, "../output/", "output path");
DEFINE_string(p, "-inf", "p");
DEFINE_string(q, "-inf", "q");

DEFINE_bool(gg, false, "run greedy DS");
DEFINE_bool(fg, false, "run fast greedy DS");
DEFINE_bool(fc, false, "run FC-based DS");
DEFINE_bool(gfc, false, "run generalized FC-based DS");
DEFINE_bool(gs, false, "get statistics of ds");
DEFINE_bool(ns, false, "get statistics of vertices");
DEFINE_bool(mp, false, "use multiple precision computation");
DEFINE_bool(h, false, "use heap-based implementation");
DEFINE_bool(info, false, "get statistics of the input graph");

DEFINE_double(beta, 2, "value of beta for FC-based DS");
DEFINE_double(eps, 0.2, "value of epsilon for fast greedy DS");
DEFINE_uint64(prec, 256, "precision for multiple precision computation using the MPFR library");


void SetStackSize(uint size = 500) {

    rlimit limit{};
    limit.rlim_cur = size * 1024 * 1024;
    limit.rlim_max = RLIM_INFINITY;
    if (setrlimit(RLIMIT_STACK, &limit)) {
        printf("Set stack size failed.\n");
        exit(-1);
    }
}

void LoadGDS(const string &file, vector<ll_uint> &gds_vtx) {
    string line;
    vector<string> gds;

    getline(ifstream(file), line);
    Split(line.substr(1, line.size() - 2), gds, false, ',');
    gds_vtx.reserve(gds.size());
    for (auto &s:gds) {
        gds_vtx.emplace_back(stoll(s));
    }
}

MLG *LoadGraph(bool print_stats = true) {

    Timer timer;
    auto mlg = MLG::LoadMLG(FLAGS_gpath);
    timer.Stop();

    cout << "Graph loaded in " << timer.GetTimeInMs() << "ms." << endl;
    if (print_stats) mlg->PrtStats();

    return mlg;
}

string GetOutputFile() {
    string output_file = FLAGS_o + FLAGS_gname + '_';
    string suffix = ".txt";
    if (FLAGS_mp) suffix = "_mp" + suffix;
    else if (FLAGS_h) suffix = "_heap" + suffix;

    if (FLAGS_gg) output_file += "greedy_" + FLAGS_q + "_" + FLAGS_p + suffix;
    else if (FLAGS_fg) output_file += "fgreedy_" + FLAGS_q + "_" + FLAGS_p + +"_" + to_string(FLAGS_eps) + suffix;
    else if (FLAGS_fc) output_file += "fc_" + to_string(FLAGS_beta) + suffix;
    else if (FLAGS_gfc) output_file += "gfc_" + FLAGS_p + "_" + to_string(FLAGS_beta) + suffix;
    else if (!FLAGS_mode.empty()) {
        output_file += "greedy_" + FLAGS_mode + "_" + FLAGS_q + "_" + FLAGS_p + suffix;
    }
    return output_file;
}

string GetOutputFile(const string &test_file, bool ns = false) {
    auto loc = test_file.find_last_of('/') + 1;
    return FLAGS_o + test_file.substr(loc, test_file.size() - loc - 4) + (ns ? "_nodestats.txt" : "_stats.txt");
}

string GetGraphInfoFile() {
    return FLAGS_o + FLAGS_gname + "_info.txt";
}

int main(int argc, char **argv) {

    SetStackSize();
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    if (FLAGS_info) {
        auto mlg = LoadGraph();
        auto out = ofstream(GetGraphInfoFile());

        out << "n = " << mlg->GetN() << endl;
        out << "m = " << mlg->GetM() << endl;
        out << "l = " << mlg->GetLn() << endl;
        out << "max_deg = " << mlg->GetMaxDeg() << endl;
        out.close();

    } else if (FLAGS_gs) { /* Get statistics of DSs */
        vector<string> files;

        Split(FLAGS_tf.substr(1, FLAGS_tf.size() - 2), files, false, ',');

        auto mlg = LoadGraph(false);
        for (auto &test_file:files) {
            vector<ll_uint> gds_vtx;
            vector<uint> gds;

            LoadGDS(test_file, gds_vtx);
            mlg->Map2VtxId(gds_vtx, gds);

            auto o_file = ofstream(GetOutputFile(test_file));
            //            GetStats(*mlg, gds, o_file, FLAGS_q, FLAGS_p, FLAGS_beta);
            GetStats(*mlg, gds, o_file);
            o_file.close();
        }

    } else if (FLAGS_ns) { /* Get statistics of DSs */
        vector<string> files;

        Split(FLAGS_tf.substr(1, FLAGS_tf.size() - 2), files, false, ',');

        auto mlg = LoadGraph(false);
        for (auto &test_file:files) {
            vector<ll_uint> gds_vtx;
            vector<uint> gds;

            LoadGDS(test_file, gds_vtx);
            mlg->Map2VtxId(gds_vtx, gds);

            auto o_file = ofstream(GetOutputFile(test_file, true));
            //            GetStats(*mlg, gds, o_file, FLAGS_q, FLAGS_p, FLAGS_beta);
            GetNodeStats(*mlg, gds, o_file);
            o_file.close();
        }

    } else {
        vector<ll_uint> gds_vtx;
        vector<uint> gds;
        double density = 0;

        auto mlg = LoadGraph();

        Timer running_timer;
        if (FLAGS_mp) { /* Use multiprecision computation */
            #if MPFR
            PREC = FLAGS_prec;

            if (FLAGS_gg) density = GreedyMp(*mlg, gds, FLAGS_q, FLAGS_p);
            else if (FLAGS_fg) density = GreedyMp_fast(*mlg, gds, FLAGS_q, FLAGS_p, FLAGS_eps);
            else if (!FLAGS_mode.empty()) {
                if (FLAGS_mode == "1qp") density = GreedyMp_1qp(*mlg, stod(FLAGS_q), stod(FLAGS_p), gds);
                else if (FLAGS_mode == "1pq") density = GreedyMp_1pq(*mlg, stod(FLAGS_q), stod(FLAGS_p), gds);
                else if (FLAGS_mode == "p1q") density = GreedyMp_p1q(*mlg, stod(FLAGS_q), stod(FLAGS_p), gds);
            }
            #else
            print("Recompile with DMPFR=On");
            return 0;
            #endif

        } else {

            if (FLAGS_gg) density = Greedy(*mlg, gds, FLAGS_q, FLAGS_p, FLAGS_h);
            else if (FLAGS_fg) density = Greedy_fast(*mlg, gds, FLAGS_q, FLAGS_p, FLAGS_eps, FLAGS_h);
            else if (FLAGS_fc) density = DensestFC(*mlg, gds, FLAGS_beta);
            else if (FLAGS_gfc) density = DensestGFC(*mlg, gds, FLAGS_p, FLAGS_beta, FLAGS_h);
            else if (!FLAGS_mode.empty()) {

                if (FLAGS_mode == "1qp")
                    density = FLAGS_h ? Greedy_1qp<MixHeap>(*mlg, stod(FLAGS_q), stod(FLAGS_p), gds)
                            : Greedy_1qp<SetHeap>(*mlg, stod(FLAGS_q), stod(FLAGS_p), gds);
                else if (FLAGS_mode == "1pq")
                    density = FLAGS_h ? Greedy_1pq<MixHeap>(*mlg, stod(FLAGS_q), stod(FLAGS_p), gds)
                            : Greedy_1pq<SetHeap>(*mlg, stod(FLAGS_q), stod(FLAGS_p), gds);
                else if (FLAGS_mode == "p1q")
                    density = FLAGS_h ? Greedy_p1q<MixHeap>(*mlg, stod(FLAGS_q), stod(FLAGS_p), gds)
                            : Greedy_p1q<SetHeap>(*mlg, stod(FLAGS_q), stod(FLAGS_p), gds);
            }
        }
        running_timer.Stop();

        auto out = ofstream(GetOutputFile());
        mlg->Map2Vtx(gds, gds_vtx);
        out << Vec2Str(gds_vtx) << endl;
        out << "runtime = " << running_timer.GetTimeInMs() << "ms" << endl;
        out << "size = " << gds_vtx.size() << endl;
        out << "density = " << density << endl;
        out.close();
    }

    return 0;
}
