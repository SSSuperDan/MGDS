import sys
from utils import *

gds_mode = '-gpath {} -gname {} -h -mode {} -q {} -p {} -o ../output/'
gds_gg = '-gpath {} -gname {} -h -gg -q {} -p {} -o ../output/'
gds_fg = '-gpath {} -gname {} -h -fg -eps {} -q {} -p {} -o ../output/'

gds_fc = '-gpath {} -gname {} -fc -beta {} -o ../output/'
gds_gfc = '-gpath {} -gname {} -gfc -beta {} -p {} -o ../output/'

case = sys.argv[1]

for d in datasets:
    dataset_path = path + d + '/'

    # MGDS-C1: p = 2 q = 1
    run([executor] + gds_mode.format(dataset_path, d, '1qp', 1, 2).split())

    # MGDS-C2: p = 1 q = 2
    run([executor] + gds_mode.format(dataset_path, d, '1pq', 2, 1).split())

    # MGDS-Fast-C1: p = 2 q = 1 eps = 0.2
    run([executor] + gds_fg.format(dataset_path, d, 0.2, 1, 2).split())

    # MGDS-Fast-C2: p = 1 q = 2 eps = 0.2
    run([executor] + gds_fg.format(dataset_path, d, 0.2, 2, 1).split())

    # MGDS-C3: p = -inf q = 1
    run([executor] + gds_gg.format(dataset_path, d, 1, '-inf').split())

    # MGDS-C3-min: p = -inf q = -inf
    run([executor] + gds_gg.format(dataset_path, d, '-inf', '-inf').split())

    # MGDS-C4: p = -1 q = 1
    run([executor] + gds_mode.format(dataset_path, d, 'p1q', 1, -1).split())

    # FC: beta = 2
    run([executor] + gds_fc.format(path + d + '/', d, 2).split())

    # GFC: p = 1 beta = 2
    run([executor] + gds_gfc.format(path + d + '/', d, 2, 1).split())



# ============ collect the results ===============
path = '../output/'

case1_file = path + '{}_greedy_1qp_1_2_heap.txt'
case2_file = path + '{}_greedy_1pq_2_1_heap.txt'

case1_fast_file = path + '{}_fgreedy_1_2_0.200000_heap.txt'
case2_fast_file = path + '{}_fgreedy_2_1_0.200000_heap.txt'

case3_file = path + '{}_greedy_1_-inf_heap.txt'
case3_min_file = path + 'effec/{}_greedy_-inf_-inf_heap.txt'
case4_file = path + '{}_greedy_p1q_1_-1_heap.txt'

fc_file = path + '{}_fc_2.000000.txt'
gfc_file = path + '{}_gfc_2_2.000000.txt'

test_files = {'MGDS-C1':case1_file, 'MGDS-C2':case2_file, 'MGDS-Fast-C1':case1_fast_file, 'MGDS-Fast-C2':case2_fast_file,
              'MGDS-C3':case3_file, 'MGDS-C4':case4_file, 'MGDS-C3-min':case3_min_file, 'FC':fc_file, 'GFC':gfc_file}

f = open('../output/runtime.txt', 'w')
for d in datasets:
    for case, tf in test_files.items():
        cf = tf.format(d)
        runtime_in_ms, size, density = parse_file(cf)
        f.write('{}\t{}\t{}\t{}\t{}\n'.format(d, case, runtime_in_ms, size, density))

f.close()
