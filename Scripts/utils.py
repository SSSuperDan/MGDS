import subprocess
from collections import defaultdict

import numpy as np

timeout_s = 60 * 60 * 24

path = '../Datasets/'
executor = '../Code/build/mlgDensest'

datasets = [ 'FAO', 'Sacchcere', 'Homo', 'Amazon', 'Higgs', 'Friendfeed', 'DBLP-small', 'DBLP-large', 'ObamaInIsrael', 'StackOverflow']


def run(cmd, env=None, stdout=None):
    # ref: https://alexandra-zaharia.github.io/posts/kill-subprocess-and-its-children-on-timeout-python/
    str_cmd = ' '.join(cmd)
    print(str_cmd)
    try:
        subprocess.run(cmd, env=env, stdout=stdout, timeout=timeout_s)

    except subprocess.TimeoutExpired:
        print(f'Timeout for \'{str_cmd}\' ({timeout_s}s) expired')
        f = open("../output/running_log.txt", 'a')
        f.write(f'Timeout for \'{str_cmd}\' ({timeout_s}s) expired\n')
        if env is not None:
            f.write("Env: " + str(env) + "\n")
        f.write("==================\n\n")
        f.close()


def to_str(arr):
    return '[{}]'.format(','.join([str(a) for a in arr]))

def get_int_value(seg):
    return int(seg[seg.find('=') + 1:].strip())


def get_time_value(seg):
    return float(seg[seg.find('=') + 1:-2].strip())


def get_float_value(seg):
    return float(seg[seg.find('=') + 1:].strip())


def get_value(seg):
    return seg[seg.find('=') + 1:].strip()


def parse_file(file):
    lines = open(file).readlines()
    runtime_in_ms = get_time_value(lines[1].strip())
    size = get_int_value(lines[2])
    density = get_float_value(lines[3])

    return runtime_in_ms, size, density

def is_number(s):
    try:
        float(s)
        return True
    except ValueError:
        pass
    try:
        import unicodedata
        unicodedata.numeric(s)
        return True
    except (TypeError, ValueError):
        pass
        return False

def load_ds_stat_data(file):
    data = defaultdict(list)
    for line in open(file).readlines():
        d, m, q, p, b, score, score_q, score_p, score_b, value = line.strip().split('\t')
        data['d'].append(d)
        data['m'].append(m)
        data['q'].append(q)
        data['p'].append(p)
        data['b'].append(b)
        data['score'].append(score)
        data['score_q'].append(score_q)
        data['score_p'].append(score_p)
        data['score_b'].append(score_b)
        data['value'].append(value)

    return data


def load_vtx_data(file, dd):
    data = defaultdict(list)
    for line in open(file).readlines():
        d, m, q, p, b, degs = line.strip().split('\t')
        if d != dd:
            continue
        data['d'].append(d)
        data['m'].append(m)
        data['q'].append(q)
        data['p'].append(p)
        data['b'].append(b)

        degs_seq = []
        vert_deg = degs.strip().split(',')
        for deg in vert_deg:
            deg_seq = list(map(int, deg.strip().split(' ')))
            degs_seq.append(deg_seq)
        data['value'].append(degs_seq)

    return data


def filter_ds_stat(data, filt):
    filtered_data = defaultdict(list)
    length = len(data['d'])
    for i in range(length):

        match = True
        for n, v in filt.items():
            if not (data[n][i] == v or (is_number(data[n][i]) and is_number(v) and abs(float(v) - float(data[n][i])) < 1e-6)):
                match = False
                break

        if match:
            filtered_data['d'].append(data['d'][i])
            filtered_data['m'].append(data['m'][i])
            filtered_data['q'].append(data['q'][i])
            filtered_data['p'].append(data['p'][i])
            filtered_data['b'].append(data['b'][i])
            filtered_data['score'].append(data['score'][i])
            filtered_data['score_q'].append(data['score_q'][i])
            filtered_data['score_p'].append(data['score_p'][i])
            filtered_data['score_b'].append(data['score_b'][i])
            filtered_data['value'].append(data['value'][i])

    return filtered_data


def filter_vtx_stat(data, filt):
    filtered_data = defaultdict(list)
    length = len(data['d'])
    for i in range(length):

        match = True
        for n, v in filt.items():
            if not (data[n][i] == v or (
                    is_number(data[n][i]) and is_number(v) and abs(float(v) - float(data[n][i])) < 1e-6)):
                match = False
                break

        if match:
            filtered_data['d'].append(data['d'][i])
            filtered_data['m'].append(data['m'][i])
            filtered_data['q'].append(data['q'][i])
            filtered_data['p'].append(data['p'][i])
            filtered_data['b'].append(data['b'][i])
            filtered_data['value'].append(data['value'][i])

    return filtered_data


def top_k(deg, i):
    value = sorted(deg)[::-1]
    return value[i] if i < len(deg) else value[-1]


def top_qk(deg, i):
    deg = sorted(deg)
    value = []
    value.append(deg[0])
    for q in [-1, -0.5, 0.5, 1, 2]:
        value.append(pow(sum([pow(d, q) if q > 0 or d > 0 else pow(1e-10, q) for d in deg]) / len(deg), 1 / q))
    value.append(deg[-1])
    return value[i] if i < len(deg) else value[-1]


format_method_name = {
    "case1": "MGDS-C1",
    "case2": "MGDS-C2",
    "case1_fast": "MGDS-Fast-C1",
    "case2_fast": "MGDS-Fast-C2",
    "case3": "MGDS-C3",
    "case4": "MGDS-C4",
    "case4_min": "MGDS-C4-min",
    "fc": "FC",
    "gfc": "GFC",
}


def get_log_limit(d_min, d_max):
    return [d_min / 100 if (d_min < d_max / 100) else d_max / 100, int(d_max * np.sqrt(10))]


def set_format_log_limit(ax, d_min, d_max):
    y_lim_min, y_lim_max = get_log_limit(d_min, d_max)

    e_min = np.log10(y_lim_min)
    e_max = np.log10(y_lim_max)
    ax.set_ylim([10 ** np.floor(e_min), 10 ** np.ceil(e_max)])
