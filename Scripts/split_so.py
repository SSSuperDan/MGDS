import os
import random
import networkx as nx

dataset_path = 'Datasets/stackoverflow/'
new_version_path = 'Datasets/'

MLGS = []
nodes = set()
files = open(dataset_path + 'mlg.conf').readlines()
for f in files:
    file_path = dataset_path + f.strip()

    g = nx.Graph()
    edges = set()
    for line in open(file_path).readlines():
        u, v = list(map(int, line.strip().split()))
        if (u, v) not in edges:
            edges.add((u, v))
            edges.add((v, u))
            nodes.add(u)
            nodes.add(v)

    for u, v in edges:
        if u < v:
            g.add_edge(u, v)
    MLGS.append(g)
    print('load {}'.format(f.strip()))
    edges.clear()

for n in nodes:
    for g in MLGS:
        g.add_node(n)

ne = [1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8]
versions = [2, 3, 4, 5, 6, 7, 8]
L = 24

nodes = list(nodes)
random.shuffle(nodes)

new_version_files = dict()
for version in versions:
    path = new_version_path + 'SO_{}/'.format(version)

    if not os.path.exists(path):
        os.mkdir(path)
    new_version_files[version] = []

    for i in range(0, L):
        new_version_files[version].append(open(path + 'l{}.txt'.format(i), 'w'))

    with open(path + 'mlg.conf', 'w') as f:
        for i in range(L):
            f.write('l{}.txt\n'.format(i))

num_of_edges = 0
fin = 0

exist = {v: False for v in nodes}
for v in nodes:
    exist[v] = True
    for i in range(0, L):
        for u in MLGS[i].neighbors(v):
            if exist[u]:
                num_of_edges += 1
                for f in range(fin, len(versions)):
                    new_version_files[f + 2][i].write('{} {}\n'.format(u, v))
                    new_version_files[f + 2][i].flush()
    if num_of_edges >= ne[fin]:
        fin += 1
        if fin == 7:
            break

for version in versions:
    for i in range(0, L):
        new_version_files[version][i].close()