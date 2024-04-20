from utils import *

gds_mode = '-gpath {} -gname {} -h -mode {} -q {} -p {} -o ../output/'
gds_fg = '-gpath {} -gname {} -h -fg -eps {} -q {} -p {} -o ../output/'

eps_range = [0.1, 0.2, 0.3, 0.4, 0.5]

for d in datasets:
    dataset_path = path + d + '/'
    run([executor] + gds_mode.format(dataset_path, d, '1qp', 1, 2).split())
    run([executor] + gds_mode.format(dataset_path, d, '1pq', 2, 1).split())

    for eps in eps_range:
        run([executor] + gds_fg.format(dataset_path, d, eps, 1, 2).split())
        run([executor] + gds_fg.format(dataset_path, d, eps, 2, 1).split())


# ============ collect the results ===============

path = '../output/'

fast_file = path + '{}_fgreedy_{}_{}_0.{}00000_heap.txt'
case1_file = path + '{}_greedy_1qp_1_2_heap.txt'
case2_file = path + '{}_greedy_1pq_2_1_heap.txt'

f = open('../output/fast_vs_case1.txt', 'w')
for d in datasets:
    cf = case1_file.format(d)
    runtime_in_ms, size, density = parse_file(cf)
    f.write('{}\t{}\t{}\t{}\t{}\n'.format(d, 0, runtime_in_ms, size, density))
    for eps in [1, 2, 3, 4, 5]:
        ff = fast_file.format(d, 1, 2, eps)
        runtime_in_ms, size, density = parse_file(ff)
        f.write('{}\t{}\t{}\t{}\t{}\n'.format(d, eps, runtime_in_ms, size, density))
f.close()

f = open('../output/fast_vs_case2.txt', 'w')
for d in datasets:
    cf = case2_file.format(d)
    runtime_in_ms, size, density = parse_file(cf)
    f.write('{}\t{}\t{}\t{}\t{}\n'.format(d, 0, runtime_in_ms, size, density))
    for eps in [1, 2, 3, 4, 5]:
        ff = fast_file.format(d, 2, 1, eps)
        runtime_in_ms, size, density = parse_file(ff)
        f.write('{}\t{}\t{}\t{}\t{}\n'.format(d, eps, runtime_in_ms, size, density))
f.close()
