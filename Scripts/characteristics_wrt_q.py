from matplotlib import pyplot as plt
import seaborn as sns
from matplotlib.cm import ScalarMappable
from matplotlib.colors import ListedColormap, BoundaryNorm

from utils import *

file = '../output/vtx_stats.txt'

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
colors = sns.color_palette('mako_r', 8)[1:]

def plot_qv(graph, case):
    fig, axes = plt.subplots(1, 3, figsize=(10, 3), sharey=True)

    data = load_vtx_data(file, graph)
    seg = 7
    qk_range = 7

    match_p = conf[case]['p'][0]
    selected_qs = [-2, 1, 2] if case == 'case1' else [1, 2]

    q_data = dict()

    for q in selected_qs:

        match_q = str(q) if q != -2 else '-inf'
        filtered_data = filter_vtx_stat(data,
                               {'d': graph, 'p': match_p, 'q': match_q, 'm': 'greedy' if case == 'case1' else
                               'fgreedy'})

        degs = defaultdict(list)
        for ln in range(0, qk_range):
            for deg in filtered_data['value'][0]:
                degs[ln].append(top_qk(deg, ln))
        q_data[q] = degs

    idx = [5, 6, 7, 8, 9, 10, 11]
    rng = [2 ** i for i in idx]

    ln_lst = []
    id_lst = []

    for qid in range(len(selected_qs)):
        ax = axes[qid]
        q = selected_qs[qid]

        degs = q_data[q]
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
                          multiple='fill', palette=clr, alpha = 0.85)

        ax.invert_xaxis()
        ax.get_legend().remove()
        lbs = ['${}$'.format(x) for x in [0, 0.25, 0.5, 0.75, 1]]
        ax.set_xticks([0, 0.25, 0.5, 0.75, 1], lbs[::-1], math_fontfamily='stix')

        ax.set_xlabel('Proportion', fontsize=14)
        ax.set_yticks([x + 0.25 for x in list(set(ln_lst))], ['$q_0$ $=$ {}'.format(x) for x in [' $-\infty$', '   $-1$', '$-0.5$', '   $0.5$', '     $1$', '     $2$', '    $\infty$']],
                      math_fontfamily='stix')
        ax.tick_params(axis='y', pad=-3)
        ax.set_ylabel(None)
        # ax.set_ylabel('$q_0$', math_fontfamily='stix', rotation=0, fontsize=16, labelpad=2)

    cmap = ListedColormap(colors[:len(rng)])
    bnds = [0] + idx
    norm = BoundaryNorm(bnds, cmap.N)
    cb_ax = fig.add_axes([0.25, 0.96, 0.6, 0.03])

    fig.colorbar(
        ScalarMappable(cmap=cmap, norm=norm),
        cax=cb_ax, orientation='horizontal',
        alpha=0.85
    )

    cb_ax.set_xlabel('$q_0$-mean degree', math_fontfamily='stix')
    cb_ax.xaxis.set_label_coords(-0.12, 1.3)
    cb_ax.set_xticks(bnds, ['$2^{}$'.format('{' + str(i) + '}') for i in bnds], math_fontfamily='stix')
    plt.tight_layout(rect=[0, 0, 1, 0.9])

    plt.savefig('Figure/figure3.pdf')

plot_qv('Friendfeed', 'case1')
