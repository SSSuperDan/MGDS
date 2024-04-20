from utils import *

gs_cmd = '-gpath {} -gname {} -info -o ../output/'

for d in datasets:
    dataset_path = path + d + '/'
    run([executor] + gs_cmd.format(dataset_path, d).split())
