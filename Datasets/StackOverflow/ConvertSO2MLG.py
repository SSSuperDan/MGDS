# Download dataset from http://konect.cc/networks/sx-stackoverflow/ or 	http://snap.stanford.edu/data/sx-Stack

import os
import datetime

raw_data = 'sx-stackoverflow.txt'
graph_data_path = 'Datasets/Stackoverflow/'


def get_hour(ts):
    a = datetime.datetime.utcfromtimestamp(ts)
    return a.hour


def parse_dataset(n_layer):
    ne = 0
    node_sets = [set() for _ in range(n_layer)]
    edge_sets = [set() for _ in range(n_layer)]

    graph_path = graph_data_path + str(n_layer) + '_layer/'

    if not os.path.exists(graph_path):
        os.mkdir(graph_path)

    graph_files = []
    for i in range(n_layer):
        graph_files.append(open(graph_path + 'l' + str(i) + '.txt', 'w'))

    c_step = 24 / n_layer

    with open(raw_data) as f:
        while True:
            line = f.readline()
            if not line:
                break

            u, v, ts = list(map(int, line.strip().split()))
            l = int(get_hour(ts) / c_step)

            if u != v and (u, v) not in edge_sets[l] and (v, u) not in edge_sets[l]:
                graph_files[l].write('{} {}\n'.format(u, v))
                node_sets[l].add(u)
                node_sets[l].add(v)
                edge_sets[l].add((u, v))
                edge_sets[l].add((v, u))
            ne += 1

    for i in range(n_layer):
        graph_files[i].close()

    with open(graph_path + 'mlg.conf', 'w') as f:
        for i in range(n_layer):
            f.write('l' + str(i) + '.txt\n')

    with open(graph_path + 'statistics.txt', 'w') as f:
        for i in range(n_layer):
            f.write('num_of_l' + str(i) + '_nodes:' + str(len(node_sets[i])) + '\n')
        for i in range(n_layer):
            f.write('num_of_l' + str(i) + '_edges:' + str(len(edge_sets[i])) + '\n')

    for i in range(n_layer):
        print('num_of_l' + str(i) + '_nodes:' + str(len(node_sets[i])))
    for i in range(n_layer):
        print('num_of_l' + str(i) + '_edges:' + str(len(edge_sets[i])))

    print(ne)


parse_dataset(24)
