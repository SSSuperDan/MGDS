import seaborn as sns
from matplotlib import pyplot as plt

from utils import *

def load_vs_file(file):
    data = dict()
    for d in datasets:
        data[d] = {'eps': [], 'time': [], 'size': [], 'density': []}
    lines = open(file).readlines()
    for line in lines:
        d, eps, time, size, density = line.strip().split('\t')
        data[d]['eps'].append(int(eps) / 10)
        data[d]['time'].append(float(time) / 1000)
        data[d]['size'].append(int(size))
        data[d]['density'].append(float(density))
    return data

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
ds = ['FAO', 'Sacchcere', 'Higgs', 'DBLP-large', 'ObamaInIsrael']
colors = ['#12007E', 'gray', '#F58073', '#1c8080', 'olive']
markers = ['o', '^', 's', 'v', 'p']

vs_case1_file = '../output/fast_vs_case1.txt'
data = load_vs_file(vs_case1_file)
fig, axs = plt.subplots(1, 2, figsize=(10, 3.5))

# sub-figure1
ax = axs[0]
for i in range(5):
    sns.lineplot(data[ds[i]], x='eps', y='time', marker=markers[i], markeredgewidth=1,
                 markeredgecolor=colors[i], markerfacecolor='None', markersize=8,
                 ax=ax, color=colors[i], label=ds[i])

ax.set_yscale('symlog')
ax.set_xticks([0, 0.1, 0.2, 0.3, 0.4, 0.5], ['$0$', '$0.1$', '$0.2$', '$0.3$', '$0.4$', '$0.5$'], math_fontfamily='stix')
ax.set_yticks([0, 1, 1e1, 1e2, 1e3, 1e4], ['$0$', '$10^0$', '$10^1$', '$10^2$', '$10^3$', '$10^4$'], math_fontfamily='stix')
ax.tick_params(axis = 'y', pad = -0.3)
ax.set_ylabel('Running Time (s)', fontsize=15 )
ax.set_xlabel('$\epsilon$', math_fontfamily='stix', labelpad=-0.1, fontsize=20)
ax.get_legend().remove()

# sub-figure2
ax = axs[1]
for i in range(5):
    data[ds[i]]['density'] = [x / max(data[ds[i]]['density']) for x in data[ds[i]]['density']]
    sns.lineplot(data[ds[i]], x='eps', y='density', marker=markers[i], markeredgewidth=1, markeredgecolor=colors[i],
                 markerfacecolor='None', markersize=8,
                 ax=ax, color=colors[i])

ax.set_xticks([0, 0.1, 0.2, 0.3, 0.4, 0.5], ['$0$', '$0.1$', '$0.2$', '$0.3$', '$0.4$', '$0.5$'],  math_fontfamily='stix')
ax.set_yticks([0.80, 0.85, 0.90, 0.95, 1.00], ['$0.80$', '$0.85$', '$0.90$', '$0.95$', '$1.00$'], math_fontfamily='stix')
ax.tick_params(axis = 'y', pad = -0.3)
ax.set_ylabel('Normalized Density', fontsize=15)
ax.set_xlabel('$\epsilon$', math_fontfamily='stix', labelpad=-0.1, fontsize=20)
ax.set_ylim(ymin=0.8)


handles, labels = axs[0].get_legend_handles_labels()
fig.legend(handles, labels, loc='upper center', columnspacing=0.8, handletextpad=0.3, frameon=False, fontsize=16,
           ncol=5, bbox_to_anchor=(0.5, 1.02))

plt.subplots_adjust(wspace=5)
plt.tight_layout(rect=[0, 0, 1, 0.9])
plt.savefig('Figure/figure9-a.pdf')


def filter(d, eps):
    for i in range(len(data[d]['eps'])):
        if data[d]['eps'][i] == eps:
            return data[d]['time'][i]

# advancement of fast when eps = 0.1
over1 = []
for d in datasets:
    f1 = filter(d, 0.1)
    n1 = filter(d, 0)
    over1.append(n1/f1)
print(over1)
print(max(over1))


vs_case2_file = '../output/fast_vs_case2.txt'
data = load_vs_file(vs_case2_file)
fig, axs = plt.subplots(1, 2, figsize=(10, 3.5))

# sub-figure1
ax = axs[0]
ds = [d for d in datasets if d not in ds]

for i in range(5):
    sns.lineplot(data[ds[i]], x='eps', y='time', marker=markers[i], markeredgewidth=1, markeredgecolor=colors[i],
                 markerfacecolor='None', markersize=8,
                 ax=ax, color=colors[i], label=ds[i])

ax.set_yscale('symlog')
ax.set_xticks([0, 0.1, 0.2, 0.3, 0.4, 0.5], ['$0$', '$0.1$', '$0.2$', '$0.3$', '$0.4$', '$0.5$'], math_fontfamily='stix')
ax.set_yticks([0, 1, 1e1, 1e2, 1e3, 1e4], ['$0$', '$10^0$', '$10^1$', '$10^2$', '$10^3$', '$10^4$'], math_fontfamily='stix')
ax.tick_params(axis = 'y', pad = -0.3)
ax.set_ylabel('Running Time (s)', fontsize=15)
ax.set_xlabel('$\epsilon$', math_fontfamily='stix', labelpad=-0.1, fontsize=20)
ax.get_legend().remove()

# sub-figure2
ax = axs[1]
for i in range(5):
    data[ds[i]]['density'] = [x / max(data[ds[i]]['density']) for x in data[ds[i]]['density']]
    sns.lineplot(data[ds[i]], x='eps', y='density', marker=markers[i], markeredgewidth=1, markeredgecolor=colors[i],
                 markerfacecolor='None', markersize=8,
                 ax=ax, color=colors[i])

ax.set_xticks([0, 0.1, 0.2, 0.3, 0.4, 0.5], ['$0$', '$0.1$', '$0.2$', '$0.3$', '$0.4$', '$0.5$'], math_fontfamily='stix')
ax.set_yticks([0.80, 0.85, 0.90, 0.95, 1.00], ['$0.80$', '$0.85$', '$0.90$', '$0.95$', '$1.00$'], math_fontfamily='stix')
ax.tick_params(axis = 'y', pad = -0.3)
ax.set_ylabel('Normalized Density', fontsize=15)
ax.set_xlabel('$\epsilon$', math_fontfamily='stix', labelpad=-0.1, fontsize=20)
ax.set_ylim(ymin=0.78, ymax = 1.02)
ax.set_yticks([0.8, 0.85, 0.9, 0.95, 1])


handles, labels = axs[0].get_legend_handles_labels()
fig.legend(handles, labels, loc='upper center', columnspacing=0.8, handletextpad=0.3, frameon=False, fontsize=16,
           ncol=5, bbox_to_anchor=(0.5, 1.02))

plt.subplots_adjust(wspace=5)
plt.tight_layout(rect=[0, 0, 1, 0.9])
plt.savefig('Figure/figure9-b.pdf')

# advancement of fast when eps = 0.1
over1 = []
for d in datasets:
    f1 = filter(d, 0.1)
    n1 = filter(d, 0)
    over1.append(n1/f1)
print(over1)
print(max(over1))