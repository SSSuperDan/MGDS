from utils import *
import seaborn as sns
import matplotlib.pyplot as plt

conf = {
    'case1': {'p': ['-inf'], 'q': [-2, -1, -0.5, 0.5, 1, 1.5, 2]},  # -2 is -inf
    'case2': {'p': ['1'], 'q': [1, 1.25, 1.5, 1.75, 2]},
    'case3': {'q': ['1'], 'p': [-2, -1, -0.5, 0.5, 1, 1.5, 2]}  # -2 is -inf
}

file = '../output/ds_stats.txt'

data = load_ds_stat_data(file)

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
colors = ['#12007E', 'gray', sns.color_palette("light:b")[2], '#F58073', '#1c8080',
          'royalblue', 'olive', 'orange']

dataset = ['FAO', 'Amazon']

fig, axes = plt.subplots(3, 2, figsize=(10, 9))

# sub-fig1

case = 'case1'
qs = conf[case]['q']
match_p = conf[case]['p'][0]
for id in [0, 1]:
    d = dataset[id]

    ax = axes[0][id]
    score_qs = []
    q_list = []
    q_value = []
    size = []
    for q in qs:
        for score_q in qs:
            match_q = str(q) if q != -2 else '-inf'
            match_score_q = str(score_q) if score_q != -2 else '-inf'

            filtered_data = filter_ds_stat(data,
                                   {'d': d, 'm': 'greedy', 'p': match_p, 'q': match_q, 'score': 'greedy_density',
                                    'score_q': match_score_q,
                                    'score_p': match_p, 'score_b': '-'})
            size_data = filter_ds_stat(data,
                               {'d': d, 'm': 'greedy', 'p': match_p, 'q': match_q, 'score': 'size',
                                'score_q': '-',
                                'score_p': '-', 'score_b': '-'})
            q_list.append(q)
            score_qs.append(score_q)
            q_value.append(float(filtered_data['value'][0]))
            size.append(float(size_data['value'][0]))

    length = len(q_list)

    idx = 0
    for q in list(set(score_qs)):

        sub_q = [q_list[i] for i in range(length) if score_qs[i] == q]
        if q > 0:  # use 1e-6 to replace 0 when q < 0, so here need correction
            sub_value = [q_value[i] for i in range(length) if score_qs[i] == q]
        else:
            def correction(val, i):
                if val < 1e-6 * size[i]:
                    return 0
                return val


            sub_value = [correction(q_value[i], i) for i in range(length) if score_qs[i] == q]

        print(q, sub_q, sub_value)
        sub_value = [v / (max(sub_value) + 1e-10) for v in sub_value]
        sub_data = {'q': sub_q, 'value': sub_value}

        sns.lineplot(sub_data, x='q', y='value', label= '${}$'.format(q if q != -2 else '-\infty'), ax=ax, color=colors[idx],
                     marker=markers[idx], markeredgewidth=1,
                     markeredgecolor=colors[idx], markerfacecolor='None', markersize=6)
        idx += 1

    ax.set_title(d)
    ax.set_xticks([-2, -1, -0.5, 0.5, 1, 2], ['$-\infty$', '$-1$', '$-0.5$', '$0.5$', '$1$', '$2$'], math_fontfamily='stix')
    yt = [0.00, 0.25, 0.50, 0.75, 1.00]
    ax.set_yticks(yt, ['$0.00$', '$0.25$', '$0.50$', '$0.75$', '$1.00$'], math_fontfamily='stix')

    ax.set_ylabel('Normalized Density', fontsize=15)
    ax.set_xlabel('$q$', math_fontfamily='stix', fontsize=15)

axes[0][0].get_legend().remove()
h, l = axes[0][1].get_legend_handles_labels()
order = [6, 5, 0, 1, 2, 3, 4]
ll = axes[0][1].legend([h[i] for i in order], [l[i] for i in order], loc='upper right', prop ={'math_fontfamily':'stix'}, fontsize=12, ncol=1,
                       bbox_to_anchor=(1.32, 1.05))
ll.set_title('$q_0$', prop ={'math_fontfamily':'stix', 'size':16})

# sub fig 2

case = 'case2'
qs = conf[case]['q']
match_p = conf[case]['p'][0]

for id in [0, 1]:
    d = dataset[id]

    ax = axes[1][id]
    score_qs = []
    q_list = []
    q_value = []
    for q in qs:
        for score_q in qs:
            match_q = str(q) if q != -2 else '-inf'
            match_score_q = str(score_q) if score_q != -2 else '-inf'

            filtered_data = filter_ds_stat(data,
                                   {'d': d, 'm': 'fgreedy', 'p': match_p, 'q': match_q, 'score': 'greedy_density',
                                    'score_q': match_score_q,
                                    'score_p': match_p, 'score_b': '-'})

            q_list.append(q)
            score_qs.append(score_q)
            q_value.append(float(filtered_data['value'][0]))

    length = len(q_list)

    idx = 0
    for q in list(set(score_qs)):
        sub_q = [q_list[i] for i in range(length) if score_qs[i] == q]
        sub_value = [q_value[i] for i in range(length) if score_qs[i] == q]
        sub_value = [v / (max(sub_value) + 1e-10) for v in sub_value]
        sub_data = {'q': sub_q, 'value': sub_value}

        sns.lineplot(sub_data, x='q', y='value', label=(q if q != -2 else '$\-infty$'), ax=ax, color=colors[idx],
                     marker=markers[idx], markeredgewidth=1,
                     markeredgecolor=colors[idx], markerfacecolor='None', markersize=6)
        idx += 1

    ax.set_title(d)
    ax.set_xticks([1, 1.25, 1.5, 1.75, 2], ['$1$', '$1.25$', '$1.5$', '$1.75$', '$2$'], math_fontfamily='stix')

    if d == 'FAO':
        ax.set_yticks([0.98, 0.99, 1.00], ['$0.98$', '$0.99$', '$1.00$'], math_fontfamily='stix')
    else:
        ax.set_yticks([0.994, 0.996, 0.998, 1.000], ['$0.994$', '$0.996$', '$0.998$', '$1.000$'], math_fontfamily='stix')

    ax.set_ylabel('Normalized Density', fontsize=15)
    ax.set_xlabel('$q$', math_fontfamily='stix', fontsize=15)

axes[1][0].get_legend().remove()
h, l = axes[1][1].get_legend_handles_labels()
order = [0, 3, 4, 2, 1]
ll = axes[1][1].legend([h[i] for i in order], [l[i] for i in order], prop ={'math_fontfamily':'stix'}, loc='upper right', fontsize=12, ncol=1,
                       bbox_to_anchor=(1.32, 0.95))
ll.set_title('$q_0$', prop ={'math_fontfamily':'stix', 'size':16})

# sub-fig3
ax = axes[2]
ps = conf['case3']['p']
match_q = conf['case3']['q'][0]


def forward(a):
    return pow(a, 5)


def inverse(a):
    return pow(a, 0.2)


for id in [0, 1]:
    d = dataset[id]

    ax = axes[2][id]
    score_ps = []
    p_list = []
    p_value = []
    for score_p in ps:
        for p in ps:
            match_p = str(p) if p != -2 else '-inf'
            match_score_p = str(score_p) if score_p != -2 else '-inf'

            filtered_data = filter_ds_stat(data,
                                   {'d': d, 'p': match_p, 'm': 'fgreedy' if p >= 1 else 'greedy', 'q': match_q,
                                    'score': 'greedy_density', 'score_q': match_q,
                                    'score_p': match_score_p, 'score_b': '-'})

            p_list.append(p)
            score_ps.append(score_p)
            p_value.append(float(filtered_data['value'][0]))

    p_data = {'p': p_list, 'score': score_ps, 'value': p_value}
    for item in zip(score_ps, p_list, p_value):
        print(item)

    length = len(p_list)

    if d == 'Amazon':
        ax.set_ylabel("Normalized Density", fontsize=15)
        ax.set_axis_off()
        pos = ax.axes.get_position()

        x0 = axes[1][1].axes.get_position().x0 - 0.005
        x1 = axes[1][1].axes.get_position().x1 - 0.015
        y0 = axes[2][0].axes.get_position().y0 - 0.035
        y1 = axes[2][0].axes.get_position().y1 - 0.02

        ax_bottom = fig.add_axes([x0, y0, x1 - x0, (y1 - y0) * 0.2])
        ax_top = fig.add_axes([x0, y0 + (y1 - y0) * 0.28, x1 - x0, (y1 - y0) * 0.66])

        ax_bottom.set_ylim(0, 0.4)
        ax_top.set_ylim(0.78, 1.015)
        ax_top.set(xlabel=None)
        ax_top.xaxis.set_ticks([], [])
        ax_top.set_yscale('function', functions=(forward, inverse))
        ax_top.spines[['bottom']].set_visible(False)
        ax_bottom.spines[['top']].set_visible(False)
        ax_bottom.set_yticks([0, 0.3], [0, 0.3])

        idx = 0
        for p in list(set(score_ps)):

            sub_p = [p_list[i] for i in range(length) if score_ps[i] == p]
            sub_value = [p_value[i] for i in range(length) if score_ps[i] == p]
            sub_value = [v / max(sub_value) for v in sub_value]
            sub_data = {'p': sub_p, 'value': sub_value}

            sns.lineplot(sub_data, x='p', y='value', ax=ax_bottom, label=p, color=colors[idx], marker=markers[idx],
                         markeredgewidth=1,
                         markeredgecolor=colors[idx], markerfacecolor='None', markersize=6, )
            sns.lineplot(sub_data, x='p', y='value', ax=ax_top, label='${}$'.format(p if p != -2 else '-\infty'), color=colors[idx],
                         marker=markers[idx],
                         markeredgewidth=1,
                         markeredgecolor=colors[idx], markerfacecolor='None', markersize=6, )
            idx += 1

        ax_top.set_ylabel('Normalized Density', fontsize=15)
        ax_top.yaxis.set_label_coords(-0.15, 0.28)
        ax_bottom.set_ylabel(None)
        ax_top.get_legend().remove()
        ax_bottom.get_legend().remove()

        ax_top.set_title(d)
        ax_bottom.set_xticks([-2, -1, -0.5, 0.5, 1, 2], ['$-\infty$', '$-1$', '$-0.5$', '$0.5$', '$1$', '$2$'],
                             math_fontfamily='stix')

        ax_top.set_xticks([-2, -1, -0.5, 0.5, 1, 2], [])
        ax_top.set_yticks([0.8, 0.9, 1.0], ['$0.8$', '$0.9$', '$1.0$'],
                          math_fontfamily='stix')

        ax_bottom.set_xlabel('$p$', math_fontfamily='stix', fontsize=15)

        h, l = ax_top.get_legend_handles_labels()
        order = [6, 5, 0, 1, 2, 3, 4]
        ll = ax_top.legend([h[i] for i in order], [l[i] for i in order], prop ={'math_fontfamily':'stix'},  loc='upper right', fontsize=12, ncol=1,
                           bbox_to_anchor=(1.32, 1.05))
        ll.set_title('$p_0$', prop ={'math_fontfamily':'stix', 'size':16})
    else:
        idx = 0
        for p in list(set(score_ps)):

            sub_p = [p_list[i] for i in range(length) if score_ps[i] == p]
            sub_value = [p_value[i] for i in range(length) if score_ps[i] == p]
            sub_value = [v / max(sub_value) for v in sub_value]
            sub_data = {'p': sub_p, 'value': sub_value}

            ax.set_yscale('function', functions=(forward, inverse))

            sns.lineplot(sub_data, x='p', y='value', ax=ax, label=p if p != -2 else '$-\infty$', color=colors[idx],
                         marker=markers[idx],
                         markeredgewidth=1,
                         markeredgecolor=colors[idx], markerfacecolor='None', markersize=6, )

            idx += 1
        ax.set_xticks([-2, -1, -0.5, 0.5, 1, 2], ['$-\infty$', '$-1$', '$-0.5$', '$0.5$', '$1$', '$2$'],
                      math_fontfamily='stix')
        ax.set_yticks([0.4, 0.6, 0.8, 1], ['$0.4$', '$0.6$', '$0.8$', '$1$'],
                      math_fontfamily='stix')
        ax.set_title(d)
        ax.set_ylabel("Normalized Density", fontsize=15)
        ax.set_xlabel('$p$', math_fontfamily='stix', fontsize=15)

axes[2][0].get_legend().remove()

plt.tight_layout(rect=[0, 0, 1, 1])
plt.savefig('Figure/figure4.pdf')

