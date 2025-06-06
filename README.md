[![Downloads](https://static.pepy.tech/badge/desbordante)](https://pepy.tech/project/desbordante)
[![Downloads](https://static.pepy.tech/badge/desbordante/month)](https://pepy.tech/project/desbordante)

<p>
   <img src="https://github.com/Desbordante/desbordante-core/assets/88928096/d687809b-5a3b-420e-a192-a1a2b6697b2a"/>
</p>

# General

Desbordante is a high-performance data profiler that is capable of discovering and validating many different patterns in data using various algorithms. 

The **Discovery** task is designed to identify all instances of a specified pattern *type* of a given dataset.

The **Validation** task is different: it is designed to check whether a specified pattern *instance* is present in a given dataset. This task not only returns True or False, but it also explains why the instance does not hold (e.g. it can list table rows with conflicting values). 

For some patterns Desbordante supports a **dynamic** task variant. The distiguishing feature of dynamic algorithms compared to classic (static) algorithms is that after a result is obtained, the table can be changed and a dynamic algorithm will update the result based just on those changes instead of processing the whole table again. As a result, they can be up to several orders of magnitude faster than classic (static) ones in some situations.

The currently supported data patterns are:
* Exact functional dependencies (discovery and validation)
* Approximate functional dependencies, with 
    - $g_1$ metric — classic AFDs (discovery and validation)
    - $\mu+$ metric (discovery)
    - $\tau$ metric (discovery)
    - $pdep$ metric (discovery)
    - $\rho$ metric (discovery)
* Probabilistic functional dependencies, with PerTuple and PerValue metrics (discovery and validation)
* Classic soft functional dependencies (with corellations), with $\rho$ metric (discovery and validation)
* Dynamic validation of exact and approximate ($g_1$) functional dependencies
* Numerical dependencies (validation)
* Graph functional dependencies (validation)
* Conditional functional dependencies (discovery)
* Inclusion dependencies
   - Exact inclusion dependencies (discovery and validation)
   - Approximate inclusion dependencies, with $g^{'}_{3}$ metric (discovery and validation)
* Order dependencies:
   - set-based axiomatization (discovery)
   - list-based axiomatization (discovery)
* Metric functional dependencies (validation)
* Fuzzy algebraic constraints (discovery)
* Differential Dependencies (discovery)
* Unique column combinations:
   - Exact unique column combination (discovery and validation)
   - Approximate unique column combination, with $g_1$ metric (discovery and validation)
* Association rules (discovery)
* Numerical association rules (discovery)
* Matching dependencies (discovery)
* Denial constraints
   - Exact denial constraints (discovery and validation)
   - Approximate denial constraints, with $g_1$ metric (discovery)

The discovered patterns can have many uses:
* For scientific data, especially those obtained experimentally, an interesting pattern allows to formulate a hypothesis that could lead to a scientific discovery. In some cases it even allows to draw conclusions immediately, if there is enough data. At the very least, the found pattern can provide a direction for further study. 
* For business data it is also possible to obtain a hypothesis based on found patterns. However, there are more down-to-earth and more in-demand applications in this case: clearing errors in data, finding and removing inexact duplicates, performing schema matching, and many more. 
* For training data used in machine learning applications the found patterns can help in feature engineering and in choosing the direction for the ablation study.
* For database data, found patterns can help with defining (recovering) primary and foreign keys, setting up (checking) all kinds of integrity constraints.

Desbordante can be used via three interfaces:
* **Console application.** This is a classic command-line interface that aims to provide basic profiling functionality, i.e. discovery and validation of patterns. A user can specify pattern type, task type, algorithm, input file(s) and output results to the screen or into a file.
* **Python bindings.** Desbordante functionality can be accessed from within Python programs by employing the Desbordante Python library. This interface offers everything that is currently provided by the console version and allows advanced use, such as building interactive applications and designing scenarios for solving a particular real-life task. Relational data processing algorithms accept pandas DataFrames as input, allowing the user to conveniently preprocess the data before mining patterns.
* **Web application.** There is a web application that provides discovery and validation tasks with a rich interactive interface where results can be conveniently visualized. However, currently it supports a limited number of patterns and should be considered more as an interactive demo.

A brief introduction to the tool and its use cases can be found [here](https://medium.com/@chernishev/exploratory-data-analysis-with-desbordante-4b97299cce07) (in English) and [here](https://habr.com/ru/company/unidata/blog/667636/) (in Russian). Next, a list of various articles and guides can be found [here](https://desbordante.unidata-platform.ru/papers). Finally, an extensive list of tutorial examples that cover each supported pattern is available [here](https://github.com/Desbordante/desbordante-core/tree/main/examples).

## Console

For information about the console interface check the [repository](https://github.com/Desbordante/desbordante-cli).

## Python bindings

Desbordante features can be accessed from within Python programs by employing the Desbordante Python library. The library is implemented in the form of Python bindings to the interface of the Desbordante C++ core library, using pybind11. Apart from discovery and validation of patterns, this interface is capable of providing valuable additional information which can, for example, describe why a given pattern does not hold. 

We want to demonstrate the power of Desbordante through examples where some patterns are extracted from tabular data, providing non-trivial insights. The patterns are quite complex and require detailed explanations, as well as a significant amount of code. This takes up quite a bit of space. Therefore, we do not include the actual code here; instead, we provide a clear (albeit simplified) definition and a link to a Colab notebook with interactive examples. The examples themselves are very detailed and allow users to understand the pattern and how to extract it using Desbordante.

1) Differential Dependencies (DD). DD is a statement of the form X -> Y, where X and Y are sets of attributes. It indicates that for any two rows, $t$ and $s$, if the attributes in $X$ are similar, then the attributes in $Y$ will also be similar. The similarity for each attribute is defined as: $diff(t[X_i], s[X_i]) \in [val_1, val_2]$,
where $t[X_i]$ is the value of attribute $X_i$ in row $t$, $val$ is a constant, and $diff$ is a function that typically calculates the difference, often through simple subtraction.
A live Python example that provides insight into the definition and demonstrates how to use this pattern in Desbordante is available [here](https://colab.research.google.com/github/Desbordante/desbordante-core/blob/main/examples/notebooks/Differential_Dependencies.ipynb).
2) Numeric Association Rules (NAR). NAR is a statement of the form X -> Y, where X and Y are conditions, specified on disjoint sets of attributes. Each condition takes a form of $A_1 \wedge A_2 \wedge \ldots \wedge A_n$, where $A_i$ is either $Attribute_i \in$ $[constant_{i}^{1}; constant_{i}^{2}]$ or $Attribute_i$ = $constant_i^3$. Furthermore, the statement includes the support (sup) and confidence (conf) values, which lie in $[0; 1]$. 
The rule can be interpreted as follows: 1) the supp share of rows in the dataset satisfies both the X and Y conditions, and 2) the conf share of rows that satisfy the X also satisfies Y.
A live Python example that provides insight into the definition and demonstrates how to use this pattern in Desbordante is available [here](https://colab.research.google.com/github/Desbordante/desbordante-core/blob/main/examples/notebooks/Numerical_Association_Rules.ipynb).
3) Matching Dependencies (MD). MD is a statement of the form X -> Y, where X and Y are sets of so-called column matches. Each column match includes: 1) a metric (e.g., Levenshtein distance, Jaccard similarity, etc.), 2) a left column, and 3) a right column. Note that this pattern may involve two tables in its column matches. Finally, each match has its own threshold, which is applied to the corresponding metric and lies in the $[0; 1]$ range. The dependency can be interpreted as follows: any two records that satisfy X will also satisfy Y.
A live Python example that provides insight into the definition and demonstrates how to use this pattern in Desbordante is available [here](https://colab.research.google.com/github/Desbordante/desbordante-core/blob/main/examples/notebooks/Matching_Dependencies.ipynb).
4) Denial Constraints (DC). A denial constraint is a statement that says: "For all pairs of rows in a table, it should never happen that some condition is true". Formally, DC $\varphi$ is a conjunction of predicates of the following form: $\forall s,t \in R, s \neq t: \textlnot (p_1 \wedge \ldots \wedge p_m)$. Each $p_k$ has the form $column_i$ $op$ $column_j$, where $op \in {>, <, \leq, \geq, =, \neq}$.
A live Python example that provides insight into the definition and demonstrates how to use this pattern in Desbordante is available [here](https://colab.research.google.com/github/Desbordante/desbordante-core/blob/main/examples/notebooks/Denial_Constraints.ipynb)

Desbordante offers examples for each supported pattern, sometimes several if the pattern is complex or needs to highlight its unique characteristics compared to others in the same family. We have mentioned only a small portion here, which is available in Colab. The rest can be found in our example [folder](https://github.com/Desbordante/desbordante-core/tree/main/examples).

Finally, Desbordante allows end users to solve various data quality problems by constructing ad-hoc Python programs, incorporating different Python libraries, and utilizing the search and validation of various patterns. To demonstrate the power of this approach, we have implemented several demo scenarios:

1) [Typo detection](https://colab.research.google.com/github/Desbordante/desbordante-core/blob/main/examples/notebooks/Desbordante_demo_scenario_1_typo_miner.ipynb)
2) [Data deduplication](https://colab.research.google.com/github/Desbordante/desbordante-core/blob/main/examples/notebooks/Desbordante_demo_scenario_2_deduplication.ipynb)
3) [Anomaly detection](https://colab.research.google.com/github/Desbordante/desbordante-core/blob/main/examples/notebooks/Desbordante_demo_scenario_3_anomaly_detection.ipynb)

[There is](https://desbordante.streamlit.app/) also an interactive demo for all of them, and all of these python scripts are [here](https://github.com/Desbordante/desbordante-core/tree/main/examples/expert). The ideas behind them are briefly discussed in this [preprint](https://arxiv.org/abs/2307.14935) (Section 3).


## Web interface

While the Python interface makes building interactive applications possible, Desbordante also offers a web interface which is aimed specifically for interactive tasks. Such tasks typically involve multiple steps and require substantial user input on each of them. Interactive tasks usually originate from Python scenarios, i.e. we select the most interesting ones and implement them in the web version. Currently, only the typo detection scenario is implemented. The web interface is also useful for pattern discovery and validation tasks: a user may specify parameters, browse results, employ advanced visualizations and filters, all in a convenient way.

You can try the deployed web version [here](https://desbordante.unidata-platform.ru/). You have to register in order to process your own datasets. Keep in mind that due to high demand various time and memory limits are enforced: processing is aborted if they are exceeded. The source code of the web interface is kept in a separate [repo](https://github.com/Desbordante/desbordante-server-node).

## I still don't understand how to use Desbordante and patterns :(

No worries! Desbordante offers a novel type of data profiling, which may require that you first familiarize yourself with its concepts and usage. The most challenging part of Desbordante are the primitives: their definitions and applications in practice. To help you get started, here’s a step-by-step guide:

1) First of all, explore the guides on our [website](https://desbordante.unidata-platform.ru/papers). Since our team currently does not include technical writers, it's possible that some guides may be missing.
2) To compensate for the lack of guides, we provide several examples for each supported pattern. These examples illustrate both the pattern itself and how to use it in Python. You can check them out [here](https://github.com/Desbordante/desbordante-core/tree/main/examples).
3) Each of our patterns was introduced in a research paper. These papers typically provide a formal definition of the pattern, examples of use, and its application scope. We recommend at least skimming through them. Don't be discouraged by the complexity of the papers! To effectively use the patterns, you only need to read the more accessible parts, such as the introduction and the example sections.
4) Finally, do not hesitate to ask questions in the mailing list (link below) or create an issue.

### Papers about patterns

Here is a list of papers about patterns, organized in the recommended reading order in each item:

* Exact functional dependencies
   - [Thorsten Papenbrock et al. 2015. Functional dependency discovery: an experimental evaluation of seven algorithms. Proc. VLDB Endow. 8, 10 (June 2015), 1082–1093.](http://www.vldb.org/pvldb/vol8/p1082-papenbrock.pdf)
   - [Thorsten Papenbrock and Felix Naumann. 2016. A Hybrid Approach to Functional Dependency Discovery. In Proceedings of the 2016 International Conference on Management of Data (SIGMOD '16). Association for Computing Machinery, New York, NY, USA, 821–833.](https://hpi.de/fileadmin/user_upload/fachgebiete/naumann/publications/PDFs/2016_papenbrock_a.pdf)
* Approximate functional dependencies ($g_{1}, \mu+, \tau, pdep, \rho$ metrics)
   - [Marcel Parciak et al. 2024. Measuring Approximate Functional Dependencies: A Comparative Study. In Proceedings 2024 IEEE 40th International Conference on Data Engineering (ICDE), Utrecht, Netherlands, 2024, pp. 3505-3518](https://arxiv.org/abs/2312.06296)
   - [Sebastian Kruse and Felix Naumann. 2018. Efficient discovery of approximate dependencies. Proc. VLDB Endow. 11, 7 (March 2018), 759–772.](https://www.vldb.org/pvldb/vol11/p759-kruse.pdf)
   - [Yka Huhtala et al. 1999. TANE: An Efficient Algorithm for Discovering Functional and Approximate Dependencies. Comput. J. 42(2): 100-111](https://dm-gatech.github.io/CS8803-Fall2018-DML-Papers/tane.pdf)
* Probabilistic functional dependencies ($PerTuple$ and $PerValue$ metrics)
   - [Daisy Zhe Wang et al. Functional Dependency Generation and Applications in Pay-As-You-Go Data Integration Systems. WebDB 2009](http://webdb09.cse.buffalo.edu/papers/Paper18/webdb09.pdf)
   - [Daisy Zhe Wang et al. Discovering Functional Dependencies in Pay-As-You-Go Data Integration Systems. Tech Rep. UCB/EECS-2009-119.](https://www2.eecs.berkeley.edu/Pubs/TechRpts/2009/EECS-2009-119.pdf)
* Classic soft functional dependencies ($\rho$ metric)
   - [Ihab F. Ilyas et al. 2004. CORDS: automatic discovery of correlations and soft functional dependencies. In Proceedings of the 2004 ACM SIGMOD international conference on Management of data (SIGMOD '04). Association for Computing Machinery, New York, NY, USA, 647–658. ](https://cs.uwaterloo.ca/~ilyas/papers/cords.pdf)
* Numerical Dependencies
   - [Paolo Ciaccia et al. 2013. Efficient derivation of numerical dependencies. Information Systems, Volume 38, Issue 3. Pages 410-429.](https://www.sciencedirect.com/science/article/abs/pii/S0306437912001044)
* Graph functional dependencies
    - [Wenfei Fan, Yinghui Wu, and Jingbo Xu. 2016. Functional Dependencies for Graphs. In Proceedings of the 2016 International Conference on Management of Data (SIGMOD '16). Association for Computing Machinery, New York, NY, USA, 1843–1857.](https://dl.acm.org/doi/pdf/10.1145/2882903.2915232)
* Conditional functional dependencies
    - [Rammelaere, J., Geerts, F. (2019). Revisiting Conditional Functional Dependency Discovery: Splitting the “C” from the “FD”. Machine Learning and Knowledge Discovery in Databases. ECML PKDD 2018. ](https://link.springer.com/chapter/10.1007/978-3-030-10928-8_33)
* Exact and approximate inclusion dependencies
    - [Falco Dürsch et al. 2019. Inclusion Dependency Discovery: An Experimental Evaluation of Thirteen Algorithms. In Proceedings of the 28th ACM International Conference on Information and Knowledge Management (CIKM '19). Association for Computing Machinery, New York, NY, USA, 219–228.](https://hpi.de/fileadmin/user_upload/fachgebiete/naumann/publications/PDFs/2019_duersch_inclusion.pdf)
    - [Sebastian Kruse, et al. Fast Approximate Discovery of Inclusion Dependencies. BTW 2017: 207-226](http://btw2017.informatik.uni-stuttgart.de/slidesandpapers/F4-10-47/paper_web.pdf)
    - [Marchi, F.D., Lopes, S. & Petit, JM. Unary and n-ary inclusion dependency discovery in relational databases. J Intell Inf Syst 32, 53–73 (2009)](https://liris.cnrs.fr/Documents/Liris-3034.pdf)
* Order dependencies:
   - [Jaroslaw Szlichta et al. 2017. Effective and complete discovery of order dependencies via set-based axiomatization. Proc. VLDB Endow. 10, 7 (March 2017), 721–732.](http://www.vldb.org/pvldb/vol10/p721-szlichta.pdf)
   - [Langer, P., Naumann, F. Efficient order dependency detection. The VLDB Journal 25, 223–241 (2016)](https://link.springer.com/article/10.1007/s00778-015-0412-3)
* Metric functional dependencies
   - [N. Koudas et al. "Metric Functional Dependencies," 2009 IEEE 25th International Conference on Data Engineering, Shanghai, China, 2009, pp. 1275-1278.](https://ieeexplore.ieee.org/document/4812519)
* Fuzzy algebraic constraints
   - [Paul G. Brown and Peter J. Hass. 2003. BHUNT: automatic discovery of Fuzzy algebraic constraints in relational data. In Proceedings of the 29th international conference on Very large data bases - Volume 29 (VLDB '03), Vol. 29. VLDB Endowment, 668–679.](https://www.vldb.org/conf/2003/papers/S20P03.pdf)
* Differential dependencies
   - [Shaoxu Song and Lei Chen. 2011. Differential dependencies: Reasoning and discovery. ACM Trans. Database Syst. 36, 3, Article 16 (August 2011), 41 pages.](https://sxsong.github.io/doc/11tods.pdf)
* Exact and approximate unique column combinations
   - [Sebastian Kruse and Felix Naumann. 2018. Efficient discovery of approximate dependencies. Proc. VLDB Endow. 11, 7 (March 2018), 759–772.](https://www.vldb.org/pvldb/vol11/p759-kruse.pdf)
* Association rules
   - [Charu C. Aggarwal, Jiawei Han. 2014. Frequent Pattern Mining. Springer Cham. pp 471.](https://link.springer.com/book/10.1007/978-3-319-07821-2)
* Numerical association rules
   - [Minakshi Kaushik, Rahul Sharma, Iztok Fister Jr., and Dirk Draheim. 2023. Numerical Association Rule Mining: A Systematic Literature Review. 1, 1 (July 2023), 50 pages.](https://arxiv.org/abs/2307.00662)
   - [Fister, Iztok & Fister jr, Iztok. 2020. uARMSolver: A framework for Association Rule Mining. 10.48550/arXiv.2010.10884.](https://doi.org/10.48550/arXiv.2010.10884)
* Matching dependencies
   - [Philipp Schirmer, Thorsten Papenbrock, Ioannis Koumarelas, and Felix Naumann. 2020. Efficient Discovery of Matching Dependencies. ACM Trans. Database Syst. 45, 3, Article 13 (September 2020), 33 pages. https://doi.org/10.1145/3392778](https://dl.acm.org/doi/10.1145/3392778)
* Denial constraints
   - [X. Chu, I. F. Ilyas and P. Papotti. Holistic data cleaning: Putting violations into context. 2013. IEEE 29th International Conference on Data Engineering (ICDE), Brisbane, QLD, Australia, 2013, pp. 458-469,](https://cs.uwaterloo.ca/~ilyas/papers/XuICDE2013.pdf)
   - [Zifan Liu, Shaleen Deep, Anna Fariha, Fotis Psallidas, Ashish Tiwari, and Avrilia Floratou. 2024. Rapidash: Efficient Detection of Constraint Violations. Proc. VLDB Endow. 17, 8 (April 2024), 2009–2021.](https://arxiv.org/pdf/2309.12436)
   - [Renjie Xiao, Zijing Tan, Haojin Wang, and Shuai Ma. 2022. Fast approximate denial constraint discovery. Proc. VLDB Endow. 16, 2 (October 2022), 269–281.](https://doi.org/10.14778/3565816.3565828)

## Installation (this is what you probably want if you are not a project maintainer)
Desbordante is [available](https://pypi.org/project/desbordante/) at the Python Package Index (PyPI). Dependencies:

* Python >=3.8

To install Desbordante type:

```sh
$ pip install desbordante
```

However, as Desbordante core uses C++, additional requirements on the machine are imposed. Therefore this installation option may not work for everyone. Currently, only manylinux2014 (Ubuntu 20.04+, or any other linux distribution with gcc 10+) and macOS 11.0+ (arm64, x86_64) is supported. If the above does not work for you consider building from sources.

## Build instructions

### Ubuntu and macOS
The following instructions were tested on Ubuntu 20.04+ LTS and macOS Sonoma 14.7+ (Apple Silicon).
### Dependencies
Prior to cloning the repository and attempting to build the project, ensure that you have the following software:

- GNU GCC, version 10+, LLVM Clang, version 16+, or Apple Clang, version 15+
- CMake, version 3.25+
- Boost library built with compiler you're going to use (GCC or Clang), version 1.85.0+

To use test datasets you will need:
- Git Large File Storage, version 3.0.2+

Instructions below are given for GCC (on Linux) and Apple Clang (on macOS).
Instructions for other supported compilers can be found in [Desbordante wiki](https://github.com/Desbordante/desbordante-core/wiki/Building).

#### Ubuntu dependencies installation (GCC)

For Ubuntu versions earlier than 24.04, you need to add the Kitware APT repository to your system 
by following their [official guide](https://apt.kitware.com) to install the latest version of CMake.

Then run the following commands:
```sh 
sudo apt update && sudo apt upgrade
sudo apt install g++ cmake ninja-build libboost-all-dev git-lfs python3 python3-venv
export CXX=g++
```
The last line sets g++ as CMake compiler in your terminal session.
You can also set it by default in all sessions: `echo 'export CXX=g++' >> ~/.profile`

For Ubuntu 24.04 and above, you can skip to the build steps. For older versions the Ubuntu APT repository
might not have a compatible version of Boost, so you'll need to install it manually:
```sh
wget https://archives.boost.io/release/1.87.0/source/boost_1_87_0.tar.gz
tar xzvf boost_1_87_0.tar.gz
cd boost_1_87_0 && ./bootstrap.sh
sudo ./b2 install --prefix=/usr/
```

#### macOS dependencies installation (Apple Clang)

Install Xcode Command Line Tools if you don't have them. Run:
```sh
xcode-select --install
```
Follow the prompts to continue.

To install CMake and Boost on macOS we recommend to use [Homebrew](https://brew.sh/) package manager. With Homebrew
installed, run the following commands:
```sh
brew install cmake boost
```
After installation, check `cmake --version`. If command is not found, then you need to add to environment path to
homebrew installed packages. To do this open `~/.zprofile` (for Zsh) or
`~/.bash_profile` (for Bash) and add to the end of the file the output of `brew shellenv`.
After that, restart the terminal and check the version of CMake again, now it should be displayed.

Run the following commands:
```sh
export CXX=clang++
export BOOST_ROOT=$(brew --prefix boost)
```
These commands set Apple Clang and Homebrew Boost as default in CMake in your terminal session.
You can also add them to the end of `~/.profile` to set this by default in all sessions.

### Building the project
#### Building the Python module using pip

Clone the repository, change the current directory to the project directory and run the following commands:

```bash
./build.sh --deps-only
python3 -m venv .venv
source .venv/bin/activate
python3 -m pip install .
```

Now it is possible to `import desbordante` as a module from within the created virtual environment. 

#### Building tests & the Python module manually
In order to build tests, pull the test datasets using the following command:
```sh
./pull_datasets.sh
```
then build the tests themselves:
```sh
./build.sh
```

The Python module can be built by providing the `--pybind` switch:
```sh
./build.sh --pybind 
```

See `./build.sh --help` for more available options.

The `./build.sh` script generates the following file structure in `/path/to/desbordante-core/build/target`:
```
├───input_data
│   └───some-sample-csv\'s.csv
├───Desbordante_test
├───desbordante.cpython-*.so
```

The `input_data` directory contains several .csv files that are used by `Desbordante_test`. Run `Desbordante_test` to perform unit testing:
```sh
cd build/target
./Desbordante_test --gtest_filter='*:-*HeavyDatasets*'
```

Alternatively, you can run tests with CTest from any directory in `Desbordante` tree:
```sh
ctest --test-dir build --exclude-regex ".*HeavyDatasets.*" -j $JOBS
```
where `$JOBS` is the desired number of concurrent jobs.

`desbordante.cpython-*.so` is a Python module, packaging Python bindings for the Desbordante core library. In order to use it, simply `import` it:
```sh
cd build/target
python3
>>> import desbordante
```

We use [easyloggingpp](https://github.com/abumq/easyloggingpp) in order to log (mostly debug) information in the core library. Python bindings search for a configuration file in the working directory, so to configure logging, create `logging.conf` in the directory from which desbordante will be imported. In particular, when running the CLI with `python3 ./relative/path/to/cli.py`, `logging.conf` should be located in `.`.

## Troubleshooting

### Git LFS
If, when cloning the repo with git lfs installed, `git clone` produces the following (or similar) error:
```
Cloning into 'Desbordante'...
remote: Enumerating objects: 13440, done.
remote: Counting objects: 100% (13439/13439), done.
remote: Compressing objects: 100% (3784/3784), done.
remote: Total 13440 (delta 9537), reused 13265 (delta 9472), pack-reused 1
Receiving objects: 100% (13440/13440), 125.78 MiB | 8.12 MiB/s, done.
Resolving deltas: 100% (9537/9537), done.
Updating files: 100% (478/478), done.
Downloading datasets/datasets.zip (102 MB)
Error downloading object: datasets/datasets.zip (2085458): Smudge error: Error downloading datasets/datasets.zip (2085458e26e55ea68d79bcd2b8e5808de731de6dfcda4407b06b30bce484f97b): batch response: This repository is over its data quota. Account responsible for LFS bandwidth should purchase more data packs to restore access.
```
delete the already cloned version, set `GIT_LFS_SKIP_SMUDGE=1` environment variable and clone the repo again:
```sh
GIT_LFS_SKIP_SMUDGE=1 git clone https://github.com/Desbordante/desbordante-core.git
```

### No type hints in IDE
If type hints don't work for you in Visual Studio Code, for example, then install stubs using the command:
```sh
pip install desbordate-stubs
```
**NOTE**: Stubs may not fully support current version of `desbordante` package, as they are updated independently.

## Cite
If you use this software for research, please cite one of our papers:
1) George Chernishev, et al. Solving Data Quality Problems with Desbordante: a Demo. CoRR abs/2307.14935 (2023).
2) George Chernishev, et al. "Desbordante: from benchmarking suite to high-performance science-intensive data profiler (preprint)". CoRR abs/2301.05965. (2023).
3) M. Strutovskiy, N. Bobrov, K. Smirnov and G. Chernishev, "Desbordante: a Framework for Exploring Limits of Dependency Discovery Algorithms," 2021 29th Conference of Open Innovations Association (FRUCT), 2021, pp. 344-354, doi: 10.23919/FRUCT52173.2021.9435469.
4) A. Smirnov, A. Chizhov, I. Shchuckin, N. Bobrov and G. Chernishev, "Fast Discovery of Inclusion Dependencies with Desbordante," 2023 33rd Conference of Open Innovations Association (FRUCT), Zilina, Slovakia, 2023, pp. 264-275, doi: 10.23919/FRUCT58615.2023.10143047.
5) Y. Kuzin, D. Shcheka, M. Polyntsov, K. Stupakov, M. Firsov and G. Chernishev, "Order in Desbordante: Techniques for Efficient Implementation of Order Dependency Discovery Algorithms," 2024 35th Conference of Open Innovations Association (FRUCT), Tampere, Finland, 2024, pp. 413-424.
6) I. Barutkin, M. Fofanov, S. Belokonny, V. Makeev and G. Chernishev, "Extending Desbordante with Probabilistic Functional Dependency Discovery Support," 2024 35th Conference of Open Innovations Association (FRUCT), Tampere, Finland, 2024, pp. 158-169.
7) A. Shlyonskikh, M. Sinelnikov, D. Nikolaev, Y. Litvinov and G. Chernishev, "Lightning Fast Matching Dependency Discovery with Desbordante," 2024 36th Conference of Open Innovations Association (FRUCT), Lappeenranta, Finland, 2024, pp. 729-740.

# Contacts and Q&A

If you have any questions regarding the tool usage you can ask it in our [google group](https://groups.google.com/g/desbordante). To contact dev team email George Chernishev, Maxim Strutovsky or Nikita Bobrov.
