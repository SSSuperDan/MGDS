from utils import *

gds_fg = '-gpath {} -gname {} -h -fg -eps {} -q {} -p {} -o ../output/'
gds_c34 = '-gpath {} -gname {} -h -mode p1q -q {} -p {} -o ../output/'


# test stackoverflow w.r.t. #layers
dataset_path = path + 'stackoverflow/'
for L in range(0, 24):

    # set new mlg.conf
    with open(dataset_path + 'mlg.conf', 'w') as f:
        for i in range(L + 1):
            f.write('l{}.txt\n'.format(i))

    # run test
    run([executor] + gds_fg.format(dataset_path, 'SO_{}_layers'.format(L), 0.2, 2, 2).split())
    run([executor] + gds_c34.format(dataset_path, 'SO_{}_layers'.format(L), 1, -1).split())


# test stackoverflow w.r.t. #edges
path = 'Datasets/'

versions = [2, 3, 4, 5, 6, 7, 8]
for version in versions:
    dataset_path = path + 'SO_{}/'.format(version)

    # run test
    run([executor] + gds_fg.format(dataset_path, 'SO_{}_edges'.format(version), 0.2, 2, 2).split())
    run([executor] + gds_c34.format(dataset_path, 'SO_{}_edges'.format(version), 1, -1).split())



# ============ collect the results ===============

path = '../output/'

# w.r.t. #layers
fast_file = path + 'SO_{}_layers_fgreedy_2_2_0.200000_heap.txt'  # 0--23
c34_file = path + 'SO_{}_layers_greedy_p1q_1_-1_heap.txt'  # 0--23

f = open('../output/sca_layer_fast.txt', 'w')
for l in range(0, 24):
    runtime_in_ms, size, density = parse_file(fast_file.format(l))
    f.write('{}\t{}\t{}\t{}\n'.format(l, runtime_in_ms, size, density))
f.close()

f = open('../output/sca_layer_case34.txt', 'w')
for l in range(0, 24):
    runtime_in_ms, size, density = parse_file(c34_file.format(l))
    f.write('{}\t{}\t{}\t{}\n'.format(l, runtime_in_ms, size, density))
f.close()


# w.r.t. #edges
fast_file = path + 'SO_{}_edges_fgreedy_2_2_0.200000_heap.txt'  # 0--23
c34_file = path + 'SO_{}_edges_greedy_p1q_1_-1_heap.txt'  # 0--23

f = open('../output/sca_edge_fast.txt', 'w')
for l in range(2, 9):
    runtime_in_ms, size, density = parse_file(fast_file.format(l))
    f.write('{}\t{}\t{}\t{}\n'.format(l, runtime_in_ms, size, density))
f.close()

f = open('../output/sca_edge_case34.txt', 'w')
for l in range(2, 9):
    runtime_in_ms, size, density = parse_file(c34_file.format(l))
    f.write('{}\t{}\t{}\t{}\n'.format(l, runtime_in_ms, size, density))
f.close()
