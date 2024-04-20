#ifndef MLGDENSEST_HEADER_H
#define MLGDENSEST_HEADER_H

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/resource.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <filesystem>

#include <dirent.h>

#include <cfenv>
#include <limits>
#include <climits>
#include <cfloat>
#include <float.h>

#include <functional>
#include <algorithm>
#include <random>
#include <cmath>

#include <unordered_map>
#include <vector>
#include <set>

#include <stdint.h>
#include <string>
#include <cstring>
#include <cassert>

#include <gflags/gflags.h>


using std::string;
using std::ifstream;
using std::ofstream;
using std::basic_ifstream;
using std::basic_ofstream;
using namespace std::filesystem;

using std::cerr;
using std::cout;

using std::stoi;
using std::sort;
using std::ceil;
using std::max;
using std::abs;
using std::endl;
using std::to_string;

using std::pair;
using std::set;
using std::unordered_map;
using std::vector;
using std::make_pair;

using uint = unsigned int;
using ll_uint = long long unsigned int;
using l_double = long double;

static uint PREC = 256;


#if MPFR
#include "mpfr.h"
static auto RND_PREC = MPFR_RNDN;
# endif

static void print(const string &info) {
    cout << info << endl;
}

static void print_debug_info(const string &info) {
#if MY_DEBUG
    cout << info << endl;
#endif
}


#endif //MLGDENSEST_HEADER_H
