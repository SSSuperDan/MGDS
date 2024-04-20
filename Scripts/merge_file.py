import sys


def merge_file(f, n):
    base = f[:f.rfind('.')]
    end = f[f.rfind('.'):]

    files = [open(base + '_{}'.format(i)+ end) for i in range(n)]

    with open(f, 'w') as f:
        for fi in files:
            for line in fi.readlines():
                f.write(line)


# merge_file('../output/vtx_stats.txt', 4)
# merge_file('../Datasets/Friendfeed/l2.txt', 3)
# merge_file('../Datasets/Higgs/l0.txt', 2)


merge_file(sys.argv[1], sys.argv[2])