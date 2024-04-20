import matplotlib.pyplot as plt
import seaborn as sns
from utils import *


def load_vs_file(file):
    data = {'dataset': [], 'method': [], 'time': [], 'size': [], 'density': []}
    lines = open(file).readlines()
    for line in lines:
        d, mtd, time, size, density = line.strip().split('\t')
        data['dataset'].append(d)
        m = format_method_name[mtd]
        if m == 'MGDS-C4-min':
            m = 'MGDS-C3-min'
        elif m == 'MGDS-C4':
            m = 'MGDS-C3'
        elif m == 'MGDS-C3':
            m = 'MGDS-C4'
        data['method'].append(m)

        data['time'].append(float(time) / 1000)
        data['size'].append(int(size))
        data['density'].append(float(density))
    return data


plt.rcParams['font.size'] = 15
# plt.rcParams['font.weight'] = 'bold'
plt.rcParams['ytick.major.size'] = 10
plt.rcParams['ytick.major.width'] = 1.5
plt.rcParams['ytick.minor.size'] = 5
plt.rcParams['ytick.minor.width'] = 1
plt.rcParams['ytick.direction'] = 'in'
plt.rcParams['xtick.direction'] = 'in'
plt.rcParams['xtick.major.size'] = 5
plt.rcParams['xtick.major.width'] = 1.5
plt.rcParams['xtick.minor.size'] = 5
plt.rcParams['xtick.minor.width'] = 1
colors = ['#12007E', 'gray', sns.color_palette('Set2')[2], sns.color_palette("light:b")[2], '#F58073', '#1c8080',
          'olive', 'royalblue', 'orange']

res_file = '../output/runtime.txt'
data = load_vs_file(res_file)
sns.set_style('whitegrid', {"grid.linestyle": '--'})

fig, ax = plt.subplots(1, 1, figsize=(18, 4))
p1 = sns.barplot(data=data, zorder=10, x='dataset', y='time', hue='method', ax=ax, palette=colors,
                 order=[d for d in datasets], hue_order=['MGDS-C1', 'MGDS-C2', 'MGDS-Fast-C1', 'MGDS-Fast-C2',
                                                                      'MGDS-C3', 'MGDS-C3-min', 'MGDS-C4', 'FC', 'GFC'])


set_format_log_limit(ax, min(data['time']), max(data['time']))
ax.set_yscale('log')
ax.set_ylim(ymax=1e6, ymin=1e-3)
ax.set_yticks([1e-2, 1e0, 1e2, 1e4, 1e6], ['$10^{}$'.format(ee) for ee in ['{-2}', 0, 2, 4, 6]], math_fontfamily ='stix', fontsize = 14)
ax.set_xlabel(None)
ax.set_ylabel('Running Time (s)', fontsize=16)



ax.legend(ncol=10, frameon=False, fontsize=16, loc='upper center', columnspacing=0.8, handletextpad=0.3,
          bbox_to_anchor=(0.5, 1.25))

p1.margins(x=0.02)
plt.tight_layout(rect=[0.025, 0, 0.95, 0.95])
plt.savefig('Figure/figure7.pdf')



# compute

def filter(d, m):
    for i in range(len(data['dataset'])):
        if data['dataset'][i] == d and data['method'][i] == m:
            return data['time'][i]

# 1. higgs, friendfeed over dblp-large on c1
ht = filter('Higgs', 'MGDS-C1')
ft = filter('Friendfeed', 'MGDS-C1')
dt = filter('DBLP-large', 'MGDS-C1')
print(ht / dt - 1, ft / dt - 1)

# 2. advancement of fast
over1 = []
over2 = []
for d in datasets:
    f1 = filter(d, 'MGDS-Fast-C1')
    f2 = filter(d, 'MGDS-Fast-C2')
    n1 = filter(d, 'MGDS-C1')
    n2 = filter(d, 'MGDS-C2')
    over1.append(n1/f1)
    over2.append(n2/f2)
print(sum(over1)/len(over1), sum(over2)/len(over2))