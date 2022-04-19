# Capacitated Vehicle routing problem (CVRP) column generation algorithm using the CVRPSEP package

A C++ implementation of a column generation algorithm for the CVRP [[1](#references)] using GUROBI's API and CVRPSEP package [[2](#references)]. In this code it is used [this fork](https://github.com/sassoftware/cvrpsep) of the CVRPSEP package which fixes some minor issues. Please refer to [[3](#references)] for further details.

In addition, this code uses the LKH heuristic [[4](#references)] to compute the TSP cost of the initial pool of columns. Note that the LKH code is distributed for academic and non-commercial use only.

<!-- ## CVRP undirected 3-index formulation

Let *G = (V, E)* be an undirected graph, *K* a set of vehicles with capacity of *Q*, and let vertex 0 be the depot and vertices *V' = V \ \{0\}* be the customers. Consider that there is a demand *di* for each *i in V'*. The undirected three-index CVRP model can be defined as below [[1](#references)]:

<p align="center">
  <img src="./pictures/cvrp-3index.png" />
</p> -->

<!-- $$
  \begin{align}
    \min & \sum\limits_{(i, j) \in E} c_{ij} \sum\limits_{k \in K} x_{ij}^k \\
    \text{subject to} & \\
    & \sum\limits_{k \in K} y_{i}^k = 1, & \forall i \in V', \\
    & \sum\limits_{k \in K} y_{0}^k = K, \\
    & \sum\limits_{(i, j) \in E} x_{ij}^k = 2y_i^k, & \forall i \in V, \forall k \in K, \\
    & \sum\limits_{i \in V} d_i y_{i}^k \leq Q, & \forall k \in K,\\
    & \sum\limits_{(i, j) \in S} x_{ij}^k \leq |S| - 1, & \forall S \subseteq V', |S| \geq 2, \forall k \in K,\\
    & y_{i}^{k} \in \{0, 1\}, & \forall i \in V, \forall k \in K, \\
    & x_{ij}^{k} \in \{0, 1\}, & \forall i, j \in V', \forall k \in K,\\
    & x_{0j}^{k} \in \{0, 1 , 2\}, & \forall j \in V', \forall k \in K.
  \end{align}
$$ -->

## Prerequisites

* CMake.

* C++17 compiler or an early version.

* Boost library (program_options)

* GUROBI solver (9 or an early version). Academics can obtain it via this [link](https://www.gurobi.com/downloads/gurobi-optimizer-eula/#Reg "Gurobi's register page").

## Compile and run instructions

Go to the source code folder and to compile type:

```sh
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

for the debug version or

```sh
cmake -H. -Bbuild
cmake --build build
```

for the release version.

To run with a configuration file:

```sh
$ ./build/cg_cvrp -f [configuration file path]
```

See the "example.cfg" file at the "input" folder for an example of the input configuration file and 

```sh
$ ./build/cg_cvrp --help
```

to see usage.

## References

**[\[1\] P. Toth and D. Vigo. The Vehicle Routing Problem, Discrete Mathematics and Applications, SIAM, 2002](https://epubs.siam.org/doi/book/10.1137/1.9780898718515)**

**[\[2\] J. Lysgaard, A.N. Letchford and R.W. Eglese. A New Branch-and-Cut Algorithm for the Capacitated Vehicle Routing Problem, Mathematical Programming, vol. 100 (2), pp. 423-445](https://pubsonline.informs.org/doi/10.1287/trsc.1060.0188)**

**[\[3\] CVRPSEP package SAS software repository.](https://github.com/sassoftware/cvrpsep)**

**[\[4\] Lin-Kernighan heuristic (LKH) for solving the traveling salesman problem.](http://webhotel4.ruc.dk/~keld/research/LKH/)**

