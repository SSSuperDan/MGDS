from matplotlib import pyplot as plt
from matplotlib.cm import ScalarMappable
from matplotlib.colors import BoundaryNorm, ListedColormap
import seaborn as sns
from utils import *

conf = {
    'case1': {'p': ['-inf'], 'q': [-2, -1, -0.5, 0.5, 1, 1.5, 2]},  # -2 is -inf
    'case2': {'p': ['1'], 'q': [1, 1.25, 1.5, 1.75, 2]},
    'case3': {'q': ['1'], 'p': [-2, -1, -0.5, 0.5, 1, 1.5, 2]}  # -2 is -inf
}

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
markers = ['o', '^', 's', 'v', 'p', '+', '*']
colors = sns.color_palette('mako_r', 8)[1:]

graph = 'Sacchcere'
file = '../output/vtx_stats.txt'

# figure 12 in appendix
data = load_vtx_data(file, graph)
fig, axes = plt.subplots(1, 3, figsize=(10, 3), sharey=True)

seg = 7
qk_range = 7

idx = [1, 2, 3, 4, 5, 6, 12]
rng = [2 ** i for i in idx]
q_data = []

# subplot 1
case = 'case2'
match_p = '1'
match_q = '1'

filtered_data = filter_vtx_stat(data,
                                {'d': graph, 'p': match_p, 'q': match_q, 'm': 'greedy' if case == 'case1' else
                                'fgreedy'})

degs = defaultdict(list)
for ln in range(0, qk_range):
    for deg in filtered_data['value'][0]:
        degs[ln].append(top_qk(deg, ln))
q_data.append(degs)

# subplot 2
filtered_data = filter_vtx_stat(data,
                                {'d': graph, 'p': '-', 'q': '-', 'm': 'fc'})

degs = defaultdict(list)
for ln in range(0, qk_range):
    for deg in filtered_data['value'][0]:
        degs[ln].append(top_qk(deg, ln))
q_data.append(degs)

# subplot 3
filtered_data = filter_vtx_stat(data,
                                {'d': graph, 'p': '2', 'q': '-', 'm': 'gfc'})

degs = defaultdict(list)
for ln in range(0, qk_range):
    for deg in filtered_data['value'][0]:
        degs[ln].append(top_qk(deg, ln))
q_data.append(degs)

for id in range(3):
    ax = axes[id]
    degs = q_data[id]

    ln_lst = []
    id_lst = []

    for ln, deg in degs.items():
        for dd in deg:
            id = 0
            while id < seg and dd > rng[id]:
                id += 1
            if id == seg:
                id -= 1
            ln_lst.append(ln + 0.5)
            id_lst.append(id)

    sel = list(set(id_lst))
    clr = [colors[i] for i in sel]
    ax = sns.histplot({'ln': ln_lst, 'value': id_lst}, binrange=[0, 8], binwidth=0.5, y='ln', hue='value', ax=ax,
                      multiple='fill', palette=clr, alpha=0.85)

    ax.invert_xaxis()
    ax.get_legend().remove()
    ax.set_xticks([0, 0.25, 0.5, 0.75, 1], ['$0$', '$0.25$', '$0.5$', '$0.75$', '$1$'][::-1], math_fontfamily='stix')
    ax.set_xlabel('Proportion', fontsize=14)
    ax.set_yticks([x + 0.25 for x in list(set(ln_lst))], ['$q_0$ $=$ {}'.format(x) for x in
                                                          [' $-\infty$', '   $-1$', '$-0.5$', '   $0.5$',
                                                           '     $1$', '     $2$', '    $\infty$']],
                  math_fontfamily='stix')
    # ax.set_yticks([x + 0.25 for x in list(set(ln_lst))], ['$-\infty$', '$-1$', '$-0.5$', '$0.5$', '$1$', '$2$', '$\infty$'], math_fontfamily='stix')
    ax.tick_params(axis='y', pad=-3)
    # ax.set_ylabel('$q_0$', math_fontfamily='stix', rotation=0, fontsize=16, labelpad=2)
    ax.set_ylabel(None)

cmap = ListedColormap(colors[:len(rng)])
bnds = [0] + idx
norm = BoundaryNorm(bnds, cmap.N)
cb_ax = fig.add_axes([0.2, 0.96, 0.6, 0.03])

fig.colorbar(
    ScalarMappable(cmap=cmap, norm=norm),
    cax=cb_ax, orientation='horizontal',
    alpha=0.85
)

cb_ax.set_xlabel('$q_0$-mean degree', math_fontfamily='stix')
cb_ax.xaxis.set_label_coords(-0.12, 1.3)
cb_ax.set_xticks(bnds, ['$2^{}$'.format('{' + str(i) + '}') for i in bnds], math_fontfamily='stix')
plt.tight_layout(rect=[0, 0, 1, 0.9])
plt.savefig('Figure/figure12.pdf')

# figure 5
colors = ['#12007E', sns.color_palette("light:b")[2], '#F58073', 'olive', 'orange', 'royalblue', '#1c8080']

configs = {'cf1': {'p': '1', 'q': '1', 'm': 'fgreedy'},
           'cf4': {'p': '-', 'q': '-', 'm': 'fc', 'b': '2'},
           'cf2': {'p': '2', 'q': '1', 'm': 'fgreedy'},
           'cf5': {'p': '2', 'q': '-', 'm': 'gfc', 'b': '2'}}

conf_name = {'cf1': 'MGDS ($p=1$)',
             'cf4': 'FC',
             'cf2': 'MGDS ($p=2$)',
             'cf5': 'GFC ($p=2)$'}

fig, ax = plt.subplots(1, 1, figsize=(10, 3))

ds = []
confs = []
value = []
for graph in datasets:
    data = load_ds_stat_data('../output/ds_stats.txt')

    spec = 0
    for name, conf in configs.items():

        s = 0
        n = 0
        l = 0
        for score in ['size', 'per_layer_edge_num']:
            filtered_data = filter_ds_stat(data,
                                           {'d': graph, 'p': conf['p'], 'q': conf['q'], 'm': conf['m'], 'score': score})
            if score == 'size':
                s = int(filtered_data['value'][0])
            else:
                ns = list(map(int, filtered_data['value'][0].strip()[1:-1].split(',')))
                l = len(ns)
                n = sum(ns)

        confs.append(conf_name[name])
        # print(graph, conf['q'], conf['p'], conf['m'], s, n, l, n / (s * l))

        if name == 'cf1':
            value.append(1)
            spec = n / (s * l)
        else:
            value.append(n / (s * l) / spec)
        ds.append(graph)

draw_data = {'dataset': ds, 'value': value, 'method': confs}
sns.barplot(draw_data, zorder=10, x='dataset', width=0.7, y='value', hue='method',
            ax=ax, palette=colors,
            order=datasets)

ax.set_xlabel(None)
ax.set_yticks([0.00, 0.25, 0.50, 0.75, 1.00], ['$0.00$', '$0.25$', '$0.50$', '$0.75$', '$1.00$'],
              math_fontfamily='stix')
ax.set_ylabel('Average Degree', fontsize=14)

ax.legend(ncol=10, prop={'math_fontfamily': 'stix'}, frameon=False, fontsize=16, loc='upper center',
          bbox_to_anchor=(0.5, 1.25))

plt.tight_layout(rect=[0, 0, 0.95, 1])
plt.savefig('Figure/figure5.pdf')

# table 4
graph = 'Sacchcere'

print('----- MGDS -----')
conf = [{'d': graph, 'p': 1, 'q': 1, 'm': 'fgreedy', 'score_p': 1, 'score': 'per_layer_density'},
        {'d': graph, 'p': 1, 'q': 1, 'm': 'fgreedy', 'score_p': 1, 'score_q': 1, 'score': 'greedy_density'},
        {'d': graph, 'p': 1, 'q': 1, 'm': 'fgreedy', 'score_p': 1, 'score_b': 2, 'score': 'layer_density'},
        {'d': graph, 'p': 1, 'q': 1, 'm': 'fgreedy', 'score_p': 2, 'score_b': 2, 'score': 'layer_density'}]

for c in conf:
    filtered_data = filter_ds_stat(data, c)
    print(filtered_data['value'][0])


print('----- FC -----')
conf = [{'d': graph, 'b': 2, 'm': 'fc', 'score_p': 1, 'score': 'per_layer_density'},
        {'d': graph, 'b': 2, 'm': 'fc', 'score_p': 1, 'score_q': 1, 'score': 'greedy_density'},
        {'d': graph, 'b': 2, 'm': 'fc', 'score_p': 1, 'score_b': 2, 'score': 'layer_density'},
        {'d': graph, 'b': 2, 'm': 'fc', 'score_p': 2, 'score_b': 2, 'score': 'layer_density'}]

for c in conf:
    filtered_data = filter_ds_stat(data, c)
    print(filtered_data['value'][0])


print('----- GFC -----')
conf = [{'d': graph, 'p': 2, 'b': 2, 'm': 'gfc', 'score_p': 1, 'score': 'per_layer_density'},
        {'d': graph, 'p': 2, 'b': 2, 'm': 'gfc', 'score_p': 1, 'score_q': 1, 'score': 'greedy_density'},
        {'d': graph, 'p': 2, 'b': 2, 'm': 'gfc', 'score_p': 1, 'score_b': 2, 'score': 'layer_density'},
        {'d': graph, 'p': 2, 'b': 2, 'm': 'gfc', 'score_p': 2, 'score_b': 2, 'score': 'layer_density'}]

for c in conf:
    filtered_data = filter_ds_stat(data, c)
    print(filtered_data['value'][0])
