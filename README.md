# MGDS

This repo contains the code, data, and scripts for the paper "Unveiling Densest Multilayer Subgraphs via Greedy Peeling".
## Datasets


The `Datasets/` folder presents the ML graphs used in the experiments (Table 3). For large graphs DBLP-large and StackOverflow, both the data source and the script to transform the original data to ML graphs are included.

## Code


### Compilation

The code used in the experiments is implemented in C++, using CMake for building. The [gflags](https://github.com/gflags/gflags) package is required for parsing the command line. In the `Code/` folder, compile the code using the following commands:
```bash
mkdir build 
cd build 
cmake ..
make
```

Once the compilation is successful, the executable is `build/mlgDensest`. We also offer a multi-precision implementation of our MGDS framework based on the [MPFR library](https://www.mpfr.org/). Enable it during compilation by using the following command:
```bash
cmake .. -DMPFR=On
```

### Arguments
##### Basic Input/Output
- `gpath`: Path to the ML graph or name of the ML graph file.
- `gname`: Graph name (used to name the output file).
- `o`: Directory for the output file.
##### Run MGDS
- `gg`: Run the MGDS framework (Algorithm 1).
- `fg`: Run MGDS-Fast for Case 1 or Case 2 (Algorithm 2).
- `mode`: Run a specific case of the MGDS framework (Algorithm 1):
    - `1qp`: Case 1
    - `1pq`: Case 2
    - `p1q`: Case 4
- `mp`: Enable the MPFR library for multi-precision computation.
- `h`: Use heap-based implementation; default is set implementation.
- `p`: Provide the value of $p$ for the vertex-oriented generalized density ($(q, p)$-density).
- `q`: Provide the value of $q$ for the vertex-oriented generalized density ($(q, p)$-density).
- `eps`: Provide the value of $\epsilon$ for MGDS-Fast.
- `prec`: Provide the precision for the multi-precision computing.
##### Run FC/GFC
- `fc`: Run the FirmCore-based densest subgraph algorithm maximizing the multilayer density.
- `gfc`: Run the generalized-FirmCore-based densest subgraph algorithm maximizing the $p$-mean multilayer density.
- `beta`: Provide the value of $\beta$ for the ($p$-mean) multilayer density.
##### Evaluate the ML Graphs or Dense ML Subgraphs
- `info`: Get statistics of the input ML graph.
- `tf`: Provide the list of dense subgraph files; the first line within each file is an array of all vertices in the subgraph.
- `gs`: Get statistics of the induced ML subgraph.
- `ns`: Get the degree vectors of vertices in the induced ML subgraph.

## Reproduce the Experiments

We provide Python scripts in the `scripts/` folder for rerunning the experiments and reproducing the figures/tables in the paper. The output of these scripts will be saved in the `output/` folder. Additionally, we also include in the folder the experimental results obtained by running the code compiled with GCC 9.4.0 and utilizing -O3 optimization on a server equipped with an Intel Xeon Gold 5218R processor and 754GB of RAM.
##### Effectiveness tests
For reproducing the results for the effectiveness test, run `run_ds_for_effec_tests.py` file to obtain two files `output/ds_stats.txt` and `output/vtx_stats.txt`, describing the statistics of the densest subgraphs and the degree vectors of vertices in these subgraphs obtained by different algorithms or parameter settings. To reproduce
- `Figure 3`: Run `characteristics_wrt_q.py`
- `Figure 4`: Run `effectiveness_of_mgds.py`
- `Table 4, Figure 5 and Figure 12 (in supplementary file)`: Run `compare_with_fc.py`
- `Figure 6`: Run `vis` in the `Case_study/` subfolder, which also holds the worldwide food trade information from 2010 retrieved from [World Bank](https://wits.worldbank.org/CountryProfile/en/Country/WLD/Year/2010/TradeFlow/EXPIMP/Partner/by-country/Product/16-24_FoodProd#)
##### Efficiency test
To reproduce
- `Figure 7`: Run `run_ds.py` to obtain the running time of the densest ML subgraph detection algorithms, which will be saved in `output/runtime.txt`. Afterward, run `ds_runtime.py` to generate Figure 7.
- `Figure 8`: Run `run_fast_test.py` to obtain the running time and $(q, p)$-density of the output of MGDS and MGDS-Fast for Case 1 and Case 2. The results will be saved in `output/fast_vs_case1.txt` and `output/fast_vs_case2.txt`. Then, run `effectiveness_of_fast` to reproduce Figure 9.
- `Figure 9`: Run `run_scalability_test.py` to obtain the running time of MGDS-Fast and MGDS-C4 on different versions of graph StackOverflow.  The results will be stored in files named `output/sca_xxx.txt`. Next, run `scalability.py` to reproduce `Figure 9` using the obtained results. 


### Note
Files `Datasets/Friendfeed/l2.txt`, `Datasets/Higgs/l0.txt`, and `output/vtx_stats.txt` exceed Github's file size limit of 100MB and are split into multiple parts (3, 2, and 4 slices, respectively). Before using them, you need to merge these pieces using the following commands:

```bash
python merge_file.py ../Datasets/Friendfeed/l2.txt 3
python merge_file.py ../Datasets/Higgs/l0.txt 2
python merge_file.py ../output/vtx_stats.txt 4
```
