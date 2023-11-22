# setup
## external components that require compiling
GEMF, the epidemic simulation software Faryad D. Sahneh and Futing Fan, available from [Kansas State University](https://ece.k-state.edu//netse/software/):
```
wget https://ece.k-state.edu/netse/software/GEMF%20C%20FUTING.zip
unzip GEMF\ C\ FUTING.zip
cd GEMF\ C\ FUTING/c/
sed -i 's/Gemfc_nrm/gemfc_nrm/' Makefile
sed -i 's/para.o common.o/para.o common.o -lm/' Makefile
sed -i '14a\#include <stddef.h>' common.h
make
sudo mv gemfc_nrm /usr/local/bin
```
Another version is available [here](https://github.com/niemasd/GEMF).

## included components that require compiling
ba_gen is a simple C program that generates an edge list in `network.txt` that is used by GEMF as the contact network:
```
wget https://raw.githubusercontent.com/nizzaneela/replication/main/src/ba_gen.c
gcc ba_gen.c -o ba_gen
sudo mv ba_gen /usr/local/bin/
```

## script and parameters
Parameters for the main analysis are in the `status.txt` and `para.txt` files in the `main` directory.

The script `main.sh` takes two arguments. The first is the number of processes to run in parallel. The second is the number of successful simulations required per process. So `./main.sh 8 100` will start 8 processes, and run each until it has produced 100 successful simulations.

`main.sh` works by making directories for each process using the parameters in `main`, then moving into each directroy in parallel, and executing `gemfc_nrm para.txt` repeatedly in each directory until the specified number of successful simulations has been produced in that directory.

# next steps
Next is to add post processing. Especially extracting and storing the relevant data from `output.txt` each run. Then generating transmiossion networks and sample time files. This is almost ready.

After that, CoaTran could be used to generate phylogenies, but something simpler and faster seems feasible.

Then the analysis of the phylogenies, which could be adapted from Pekar et al. but is probably better done again from scratch. 




