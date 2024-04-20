import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
from matplotlib.ticker import MultipleLocator

fast_layer_res = '../output/sca_layer_fast.txt'
case34_layer_res = '../output/sca_layer_case34.txt'
fast_edge_res = '../output/sca_edge_fast.txt'
case34_edge_res = '../output/sca_edge_case34.txt'

res = [fast_layer_res, case34_layer_res, fast_edge_res, case34_edge_res]
datas = []


def get_log_minor_tick_loc(major_ticks):
    minor_ticks = [m * 5 for m in major_ticks]
    return minor_ticks


for i in range(len(res)):
    res_file = res[i]

    l = []
    t = []
    s = []
    d = []

    lid = 0
    for line in open(res_file).readlines():
        ll, tt, ss, dd = line.strip().split()
        if (i in [0, 1] and lid % 2 == 0) or i in [2, 3]:
            if i in [0, 1]:
                l.append(int(ll) + 1)  # convert from 0-23 to 1-24
            else:
                l.append(int(ll))
            t.append(float(tt) / 1000)  # convert from ms to s
            s.append(int(ss))
            d.append(float(dd))
        lid += 1

    data = {'x': l, 'time': t, 'size': s, 'density': d}
    if i == 2 or i == 3:
        data = {k: v[:-1] for k, v in data.items()}

    datas.append(data)

plt.rcParams['font.size'] = 12
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

sns.set_style('whitegrid', {"grid.linestyle": '--'})
fig, axs = plt.subplots(1, 2, figsize=(6, 3))

colors = ['#1c8080', '#12007E', '#F58073']

# sub-figure1
ax = axs[0]

sns.lineplot(datas[0], x='x', y='time', label='MGDS-Fast', marker='o', markeredgewidth=1, markeredgecolor=colors[1],
             markerfacecolor='None', markersize=4,
             ax=ax, color=colors[1])
sns.lineplot(datas[1], x='x', y='time', label='MGDS-C4', marker='s', markeredgewidth=1, markeredgecolor=colors[2],
             markerfacecolor='None', markersize=4,
             ax=ax, color=colors[2])

ax.set_ylabel('Running Time (s)', fontsize=13, labelpad=-0.05)
ax.set_xlabel('$|L|$', math_fontfamily='stix', fontsize=15)
ax.set_ylim(ymin=0)
ml = MultipleLocator(12.5)
ax.yaxis.set_minor_locator(ml)

# xt = [1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23]
xt = [1, 5, 9, 13, 17, 21, 24]
ax.set_xticks(xt, ['${}$'.format(x) for x in xt], math_fontfamily='stix')
yt = [0, 50, 100, 150]
ax.set_yticks(yt, ['${}$'.format(y) for y in yt], math_fontfamily='stix')

ax.grid(linestyle='--', axis='y', which='major')
ax.get_legend().remove()

# sub-figure2
ax = axs[1]

sns.lineplot(datas[2], x='x', y='time', label='MGDS-Fast', marker='o', markeredgewidth=1, markeredgecolor=colors[1],
             markerfacecolor='None', markersize=5,
             ax=ax, color=colors[1])
sns.lineplot(datas[3], x='x', y='time', label='MGDS-C4', marker='s', markeredgewidth=1, markeredgecolor=colors[2],
             markerfacecolor='None', markersize=5,
             ax=ax, color=colors[2])

ax.tick_params(axis = 'y', pad = -0.3)
ax.set_ylabel('Running Time (s)', fontsize=13)
ax.set_xlabel('$|E|$', math_fontfamily='stix', fontsize=15)
ax.set_yscale('log')


exp = [2, 3, 4, 5, 6, 7]
ax.set_xticks(exp, ['$10^{}$'.format(e) for e in exp], math_fontfamily='stix')
yt = ['{-3}', '{-2}', '{-1}', 0, 1, 2]
ax.set_yticks([1e-3, 1e-2, 1e-1, 1e0, 1e1, 1e2], ['$10^{}$'.format(y) for y in yt], math_fontfamily='stix')


ax.grid(linestyle='--', axis='y', which='major')
ax.get_legend().remove()
# only need to complete the second figure


handles, labels = axs[0].get_legend_handles_labels()

fig.legend(handles, labels, loc='upper center', frameon=False, fontsize=14, ncol=2, bbox_to_anchor=(0.5, 1))
plt.tight_layout(rect=[0, 0, 1, 0.9])
plt.savefig('Figure/figure8.pdf')
