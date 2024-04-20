# Retrieve datasets from http://konect.cc/networks/dblp_coauthor

import os
from datetime import datetime

base_path = os.path.dirname(os.path.realpath(__file__)) + '/'
raw_data = base_path + 'dblp_coauthor/out.dblp_coauthor'
graph_path = base_path + 'Datasets/DBLP_large/'


def get_year(ts):
    a = datetime.utcfromtimestamp(ts)
    return a.year


def get_month(ts):
    a = datetime.utcfromtimestamp(ts)
    return a.month


def find_ym(file, col, skip_first, use_year, factor=1):
    ym_dict = dict()

    col = col - 1
    with open(file) as f:

        if skip_first:
            _ = f.readline()

        while True:
            line = f.readline()
            if not line:
                break

            seg = list(map(int, line.strip().split()))
            ts = seg[col]

            if use_year:
                y = int(get_year(ts) / factor)
                if y not in ym_dict:
                    ym_dict[y] = len(ym_dict)
            else:  # use_month
                m = get_month(ts)
                if m not in ym_dict:
                    ym_dict[m] = len(ym_dict)

    return ym_dict


def parse_by_ym(file, ts_col, skip_first, output_dir, by_year, factor):
    if not os.path.exists(output_dir):
        os.mkdir(output_dir)

    ym_dict = find_ym(file, ts_col, skip_first, by_year, factor)
    n_layer = len(ym_dict)

    node_sets = [set() for _ in range(n_layer)]
    edge_sets = [set() for _ in range(n_layer)]

    graph_files = []
    for i in range(n_layer):
        graph_files.append(open(output_dir + 'l' + str(i) + '.txt', 'w'))

    ts_col = ts_col - 1
    with open(file) as f:

        if skip_first:
            _ = f.readline()

        while True:
            line = f.readline()
            if not line:
                break

            seg = list(map(int, line.strip().split()))
            u = seg[0]
            v = seg[1]
            ts = seg[ts_col]

            if by_year:
                l = ym_dict[int(get_year(ts) / factor)]
            else:  # by month
                l = ym_dict[get_month(ts)]

            if u != v and (u, v) not in edge_sets[l] and (v, u) not in edge_sets[l]:
                graph_files[l].write('{} {}\n'.format(u, v))
                node_sets[l].add(u)
                node_sets[l].add(v)
                edge_sets[l].add((u, v))
                edge_sets[l].add((v, u))

    for i in range(n_layer):
        graph_files[i].close()

    with open(output_dir + 'mlg.conf', 'w') as f:
        for i in range(n_layer):
            f.write('l' + str(i) + '.txt\n')

    with open(output_dir + 'statistics.txt', 'w') as f:
        for i in range(n_layer):
            f.write('l{}: #nodes = {}, #edges = {}\n'.format(i, len(node_sets[i]), len(edge_sets[i])))

    for i in range(n_layer):
        print('l{}: #nodes = {}, #edges = {}'.format(i, len(node_sets[i]), len(edge_sets[i])))


def remove_tiny_layers(output_dir):
    new_layers = set()

    with open(output_dir + 'mlg.conf') as f:
        layers = f.readlines()

        for layer in layers:
            layer = layer.strip()
            n_edges = len(open(output_dir + layer).readlines())
            if n_edges >= 100:
                new_layers.add(layer)

    if len(new_layers) != len(layers):
        os.rename(output_dir + 'mlg.conf', output_dir + 'mlg.conf(old)')

        with open(output_dir + 'mlg.conf', 'w') as f:
            for layer in new_layers:
                f.write(layer + '\n')


parse_by_ym(raw_data, 4, True, graph_path, True, 3)
remove_tiny_layers(graph_path)
