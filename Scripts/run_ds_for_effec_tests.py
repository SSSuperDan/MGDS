import os

from utils import *

gds = '-gpath {} -gname {} -h -gg -q {} -p {} -o ../output/'
gds_fast = '-gpath {} -gname {} -h -fg -eps 0.1 -q {} -p {} -o ../output/'
gds_fc = '-gpath {} -gname {} -fc -beta {} -o ../output/'
gds_gfc = '-gpath {} -gname {} -gfc -beta {} -p {} -o ../output/'

for d in datasets:

    p = '-inf'
    for q in [-1, -0.5, 0.5, 1, 1.5, 2]:
        graph_path = path + d + '/'
        run([executor] + gds.format(graph_path, d, q, p).split())

    p = 1
    for q in [1.25, 1.5, 1.75, 2]:
        graph_path = path + d + '/'
        run([executor] + gds_fast.format(graph_path, d, q, p).split())

    q = 1
    for p in [-1, -0.5, 0.5, 1, 1.5, 2]:
        graph_path = path + d + '/'
        run([executor] + gds_fast.format(graph_path, d, q, p).split())

    # fc + gfc
    run([executor] + gds_fc.format(path + d + '/', d, 2).split())
    run([executor] + gds_gfc.format(path + d + '/', d, 2, 1).split())

# ================================================================
# Compute the statistics of the obtained dense ML subgraph and
# the degree vector of the vertices in the subgraph.

res_path = '../output/'
gds_ds_stat = '-gpath {} -gname {} -gs -tf [{}] -o ../output/'
gds_vtx_stat = '-gpath {} -gname {} -ns -tf [{}] -o ../output/'

case12_fast_file = res_path + '{}_fgreedy_{}_{}_0.100000_heap.txt'
case34_file = res_path + '{}_greedy_p1q_{}_{}_heap.txt'
case4_file = res_path + '{}_greedy_{}_-inf_heap.txt'
case4_min_file = res_path + '{}_greedy_-inf_-inf_heap.txt'
fc_file = res_path + '{}_fc_2.000000.txt'
gfc_file = res_path + '{}_gfc_2_2.000000.txt'

for d in datasets:
    graph_path = path + d + '/'

    tfs = [  # fix p = -inf, vary q \in [-inf, -1, -0.5, 0.5, 1, 1.5, 2]
        case4_min_file.format(d),
        case4_file.format(d, -1),
        case4_file.format(d, -0.5),
        case4_file.format(d, 0.5),
        case4_file.format(d, 1),
        case4_file.format(d, 1.5),
        case4_file.format(d, 2),
        # fix p = 1, vary q \in [1, 1.25, 1.5, 1.75, 2]
        case12_fast_file.format(d, 1, 1),
        case12_fast_file.format(d, 1.25, 1),
        case12_fast_file.format(d, 1.5, 1),
        case12_fast_file.format(d, 1.75, 1),
        case12_fast_file.format(d, 2, 1),
        # fix q = 1, vary p \in [-inf, -1, -0.5, 0.5, 1, 1.5, 2]
        case34_file.format(d, 1, -1),
        case34_file.format(d, 1, -0.5),
        case34_file.format(d, 1, 0.5),
        case12_fast_file.format(d, 1, 1.5),
        case12_fast_file.format(d, 1, 2),
        # fc, gfc
        fc_file.format(d),
        gfc_file.format(d)
    ]

    for file in tfs:
        if not os.path.exists(file):
            print('File "{}" doesn\'t exists.'.format(file))

    run([executor] + gds_ds_stat.format(graph_path, d, ','.join(tfs)).split())
    run([executor] + gds_vtx_stat.format(graph_path, d, ','.join(tfs)).split())


# ============== Collect the results ===============================

def parse(file):
    data = dict()
    for line in open(file).readlines():
        loc = line.rfind('=')
        conf = line[:loc].strip()
        value = line[loc + 1:].strip()
        data[conf] = value
    return data


class info:
    def __init__(self, d_, m_, q_, p_, b_):
        self.d = d_
        self.method = m_
        self.q = q_
        self.p = p_
        self.b = b_
        self.score = '-'
        self.score_q = '-'
        self.score_p = '-'
        self.score_b = '-'

    def flush(self, f, value):
        f.write(
            '{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\t{}\n'.format(self.d, self.method, self.q, self.p, self.b, self.score,
                                                              self.score_q, self.score_p, self.score_b, value))


# ============= collect the statistics of the ds ===============
res_path = '../output/'
case12_fast_file = res_path + '{}_fgreedy_{}_{}_0.100000_heap_stats.txt'
case34_file = res_path + '{}_greedy_p1q_{}_{}_heap_stats.txt'
case4_file = res_path + '{}_greedy_{}_-inf_heap_stats.txt'
case4_min_file = res_path + '{}_greedy_-inf_-inf_heap_stats.txt'
fc_file = res_path + '{}_fc_2.000000_stats.txt'
gfc_file = res_path + '{}_gfc_2_2.000000_stats.txt'

f = open('../output/ds_stats.txt', 'w')

for d in datasets:

    tfs = [
        case4_min_file.format(d),
        case4_file.format(d, -1),
        case4_file.format(d, -0.5),
        case4_file.format(d, 0.5),
        case4_file.format(d, 1),
        case4_file.format(d, 1.5),
        case4_file.format(d, 2),
        case12_fast_file.format(d, 1, 1),
        case12_fast_file.format(d, 1.25, 1),
        case12_fast_file.format(d, 1.5, 1),
        case12_fast_file.format(d, 1.75, 1),
        case12_fast_file.format(d, 2, 1),
        case34_file.format(d, 1, -1),
        case34_file.format(d, 1, -0.5),
        case34_file.format(d, 1, 0.5),
        case12_fast_file.format(d, 1, 1.5),
        case12_fast_file.format(d, 1, 2),
        fc_file.format(d),
        gfc_file.format(d)]

    for file in tfs:
        seg = file[file.rfind('/') + 1:].strip().split('_')
        method = file.strip().split('_')[1]
        q = '-'
        p = '-'
        b = '-'

        if method == 'fgreedy':
            q = seg[2]
            p = seg[3]
        elif seg[2] == 'p1q':
            q = seg[3]
            p = seg[4]
        elif seg[3] == '-inf':
            p = '-inf'
            q = seg[2]
        elif method == 'fc':
            b = seg[2]
        elif method == 'gfc':
            p = seg[2]
            b = seg[3]

        data = parse(file)

        for k, v in data.items():
            score_data = info(d, method, q, p, b)
            if k.count('=') > 0:
                m, c = k.split(' ', 1)
                score_data.score = m

                if c.count(',') > 0:
                    # 2 parameter:
                    p1, p2 = c.split(',')
                    p1 = p1[p1.find('(') + 1:]
                    p2 = p2.strip()[:p2.strip().find(')')]
                    n1, pv1 = p1.split('=')
                    n1 = n1.strip()
                    pv1 = pv1.strip()
                    n2, pv2 = p2.split('=')
                    n2 = n2.strip()
                    pv2 = pv2.strip()

                    assert n1 == 'p'
                    score_data.score_p = pv1
                    if n2 == 'b':
                        score_data.score_b = pv2
                    elif n2 == 'q':
                        score_data.score_q = pv2

                else:
                    # 1 parameter:
                    n, pv = c.strip()[1:-1].split('=')
                    n = n.strip()
                    pv = pv.strip()

                    assert n == 'p'
                    score_data.score_p = pv

            else:
                score_data.score = k

            score_data.flush(f, v)

f.close()


# ============= collect the degree vector of the vertices ===============

def parse(file):
    return [' '.join(line.strip()[1:-1].split(',')) for line in open(file).readlines()]

res_path = '../output/'
case12_fast_file = res_path + '{}_fgreedy_{}_{}_0.100000_heap_nodestats.txt'
case34_file = res_path + '{}_greedy_p1q_{}_{}_heap_nodestats.txt'
case4_file = res_path + '{}_greedy_{}_-inf_heap_nodestats.txt'
case4_min_file = res_path + '{}_greedy_-inf_-inf_heap_nodestats.txt'
fc_file = res_path + '{}_fc_2.000000_nodestats.txt'
gfc_file = res_path + '{}_gfc_2_2.000000_nodestats.txt'

f = open('../output/vtx_stats.txt', 'w')

for d in datasets:

    tfs = [
        case4_min_file.format(d),
        case4_file.format(d, -1),
        case4_file.format(d, -0.5),
        case4_file.format(d, 0.5),
        case4_file.format(d, 1),
        case4_file.format(d, 1.5),
        case4_file.format(d, 2),
        case12_fast_file.format(d, 1, 1),
        case12_fast_file.format(d, 1.25, 1),
        case12_fast_file.format(d, 1.5, 1),
        case12_fast_file.format(d, 1.75, 1),
        case12_fast_file.format(d, 2, 1),
        case34_file.format(d, 1, -1),
        case34_file.format(d, 1, -0.5),
        case34_file.format(d, 1, 0.5),
        case12_fast_file.format(d, 1, 1.5),
        case12_fast_file.format(d, 1, 2),
        fc_file.format(d),
        gfc_file.format(d)]

    for file in tfs:
        seg = file[file.rfind('/') + 1:].strip().split('_')
        method = seg[1]
        q = '-'
        p = '-'
        b = '-'

        if method == 'fgreedy':
            q = seg[2]
            p = seg[3]
        elif seg[2] == 'p1q':
            q = seg[3]
            p = seg[4]
        elif seg[3] == '-inf':
            p = '-inf'
            q = seg[2]
        elif method == 'fc':
            b = seg[2]
        elif method == 'gfc':
            p = seg[2]
            b = seg[3]

        lines = parse(file)
        f.write('{}\t{}\t{}\t{}\t{}\t{}\n'.format(d, method, q, p, b, ','.join(lines)))

f.close()
