#ifndef MLGDENSEST_STROPS_H
#define MLGDENSEST_STROPS_H

static void Split(const string &str, vector<string> &tokens, bool skip_first, char delim = ' ') {
    size_t last_pos, pos;

    tokens.clear();

    last_pos = str.find_first_not_of(delim, 0);
    if (skip_first) {
        last_pos = str.find(delim, last_pos);
        last_pos = str.find_first_not_of(delim, last_pos);
    }

    while (last_pos != string::npos) {
        pos = str.find(delim, last_pos);
        tokens.emplace_back(move(str.substr(last_pos, pos - last_pos)));
        last_pos = str.find_first_not_of(delim, pos);
    }
}


static bool IsDigit(const string &str) {
    auto begin = str.starts_with('-') ? str.begin() + 1 : str.begin();
    auto dot_cnt = std::count(begin, str.end(), '.');
    return (!dot_cnt && all_of(begin, str.end(), ::isdigit)) ||
    (dot_cnt == 1 && all_of(begin, str.end(), [](const char &c) { return ::isdigit(c) || c == '.'; }));
}


#endif //MLGDENSEST_STROPS_H
