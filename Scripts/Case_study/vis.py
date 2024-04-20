from collections import defaultdict
import seaborn as sns
import matplotlib.pyplot as plt
from matplotlib.colors import LinearSegmentedColormap
from matplotlib.ticker import FixedFormatter
import networkx as nx

plt.rcParams['font.size'] = 12
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

fig, axs = plt.subplots(1, 2, figsize=(12, 5), width_ratios=[1.9, 1.1])

# draw left
ax = axs[1]

clist = [plt.get_cmap('gray_r')(0.01),
         plt.get_cmap('gray_r')(0.05),
         plt.get_cmap('gray_r')(0.1),
         plt.get_cmap('gray_r')(0.2),
         plt.get_cmap('gray_r')(0.3),
         plt.get_cmap('OrRd')(0.5),
         plt.get_cmap('OrRd')(0.7),
         plt.get_cmap('OrRd')(0.99)]

cmap = LinearSegmentedColormap.from_list('custom_map', clist)
print(cmap(0.1), cmap(0.9)) # the larger, the "reder"

node_dict = dict()
lines = open('fao_trade_nodes.txt').readlines()[1:]
for line in lines:
    nid, n = line.strip().split()
    node_dict[nid] = n

# load base_node (ds in single graphs)
line = open('fao-single_greedy_1_-inf.txt').readline()
base_ds = list((line.strip()[1:-1].split(',')))
print(len(base_ds))

# load gds_node
line = open('fao_greedy_1_-inf.txt').readline()
gds = set(line.strip()[1:-1].split(','))

# rearrange nodes
base_nodes = set(base_ds) - gds
base_ds = list(sorted([int(n) for n in base_nodes]))
g_ds = list(sorted([int(n) for n in gds]))
base_ds.extend(g_ds)
base_ds = list(map(str, base_ds))

# select nodes
base_ds = [base_ds[i] for i in range(len(base_ds)) if i % 2 == 0]
gds = set([v for v in gds if v in base_ds])
print(len(base_ds))
# reorder nodes
node_loc = {base_ds[i]: i for i in range(len(base_ds))}


def swap(a, b):
    loc_a = node_loc[a]
    loc_b = node_loc[b]
    base_ds[loc_a] = b
    base_ds[loc_b] = a
    node_loc[a] = loc_b
    node_loc[b] = loc_a


swap('5', '13')
swap('108', '182')
swap('87', '158')
swap('74', '150')
swap('62', '5')
swap('43', '58')
swap('33', '98')
swap('76', '43')

#################################################
# load graph
g = defaultdict(set)
node_color = ['red' if n in gds else 'gray' for n in base_ds]
for line in open('fao_trade_multiplex.edges').readlines():
    l, u, v, _ = line.strip().split()
    if u == v:
        continue

    u, v = min(u, v), max(u, v)
    if u in base_ds and v in base_ds:
        # if u in gds and v in gds:
        g['{}-{}'.format(u, v)].add(l)

g = {k: len(v) for k, v in g.items()}
max_weight = max(g.values())

# sort edge
edges = [(edge_id, w) for edge_id, w in g.items()]
edges = sorted(edges, key=lambda x: x[1])
print(edges)

graph = nx.Graph()
for n in base_ds:
    graph.add_node(n)

id = 0
for edge_id, w in edges:
    if id % 1 == 0:
        u, v = edge_id.split('-')
        graph.add_edge(u, v, weight=w)
    id += 1

edge_color = []
for u, v, w in graph.edges(data=True):
    edge_color.append(cmap(w['weight'] / max_weight))  # min(w['weight'] / 200, 1)))

e1 = []
e2 = []
ec1 = []
ec2 = []

# order edges in the figure
idx = 0
for u, v, w in graph.edges(data=True):
    if w['weight'] < 190:
        e1.append((u, v, w))
        ec1.append(edge_color[idx])

    else:
        e2.append((u, v, w))
        ec2.append(edge_color[idx])

    idx += 1

pos = nx.circular_layout(graph)
nx.draw_networkx_nodes(graph, pos, ax=axs[0], node_color=node_color, node_size=20)
nx.draw_networkx_edges(graph, pos, ax=axs[0], edgelist=e1, edge_color=ec1, width=0.6)
nx.draw_networkx_edges(graph, pos, ax=axs[0], edgelist=e2, edge_color=ec2, width=0.6)
axs[0].axis(False)

# format node label (country name)
ns = dict()
for line in open('region_name_abbrev.txt').readlines()[2:]:
    f, s = line.strip().split('-', 1)
    ns[f.strip()] = s.strip()

labels = {}
for n in gds:
    labels[n] = ns[node_dict[n]]

##### for debug, label of v is v #####
lbs = {v: v for v in graph.nodes}
######################################

# add node label
description = nx.draw_networkx_labels(graph, pos, ax=axs[0], font_weight='bold', font_size=12, labels=labels)
r = fig.canvas.get_renderer()
trans = plt.gca().transData.inverted()
for node, t in description.items():
    pos = t.get_position()
    alpha = 1.08
    new_pos = (pos[0] * alpha, pos[1] * alpha)
    t.set_position(new_pos)

# add colorbar
sm = plt.cm.ScalarMappable(cmap=cmap, norm=plt.Normalize(vmin=0, vmax=1))

labels = [0, 50, 100, 150, 200, 250, 300]
ticks = [t / max_weight for t in labels]
lbs = ['${}$'.format(l) for l in labels]
labels = FixedFormatter(labels)
clb = plt.colorbar(sm, ax=axs[0], pad=-0.001, shrink=0.6, ticks=ticks, format=labels)
clb.ax.set_yticks(ticks, lbs, fontsize = 15,  math_fontfamily = 'stix')
clb.ax.set_ylabel('$l$', labelpad = 10, fontsize = 16, math_fontfamily = 'stix', rotation='horizontal')
# clb.ax.yaxis.set_label_coords(2.5, -0.01)
clb.ax.tick_params(labelsize=12)
clb.ax.invert_yaxis()

# plt.tight_layout()
# plt.show()

sns.set_style('whitegrid', {'grid.linestyle':'--'})
axs[1].axis(False)
l = ax.figure.subplotpars.left - 0.2
r = ax.figure.subplotpars.right
t = ax.figure.subplotpars.top
b = ax.figure.subplotpars.bottom

ax = fig.add_axes([l + (r-l) * 0.75, b + (t-b) * 0.12, (r - l) * 0.33, (t - b) * 0.8])

colors = ['#12007E', '#1c8080', '#F58073']  # sub-figure1 ax = axs[0]

# load edge weight data
data = {'n_layers': []}
gds_data = {'n_layers': []}

for e in edges:
    u, v = e[0].split('-')
    if u in gds and v in gds:
        gds_data['n_layers'].append(e[1])
    data['n_layers'].append(e[1])

width = 15
colors[1] = plt.get_cmap('gray')(0.5)
# colors = [cmap(0.1), plt.get_cmap('Blues_r')(0.5), plt.get_cmap('Reds_r')(0.5)]
sns.histplot(data, color=colors[1], label='$DS_\mathsf{single}$', ax=ax, binwidth=width, x="n_layers", stat='proportion',
             element='step', alpha=0.5)
sns.histplot(gds_data, color=colors[2], label='$DS_\mathsf{multi}$', ax=ax, binwidth=width, x="n_layers", stat='proportion',
             element='step', alpha=0.5)
xt = [0, 100, 200, 300]
yt = [0.0, 0.1, 0.2, 0.3, 0.4]
ax.set_xticks(xt, ['${}$'.format(x) for x in xt], math_fontfamily = 'stix', fontsize = 15)
ax.set_yticks(yt, ['$0.0$', '$0.1$', '$0.2$', '$0.3$', '$0.4$'], math_fontfamily = 'stix', fontsize = 15)

ax.set_xlabel('$l$', fontsize=16, math_fontfamily = 'stix')
ax.set_ylabel('Proportion', fontsize=15)
plt.legend(fontsize = 16)
plt.subplots_adjust(wspace=1)

plt.tight_layout(rect = [0, 0, 0.95, 1])
plt.savefig('../Figure/figure6.png', dpi = 300)
