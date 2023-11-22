# requirements
## external components that require compiling
GEMF, the epidemic simulation software Faryad D. Sahneh and Futing Fan, available from [Kansas State University](https://ece.k-state.edu//netse/software/).
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
ba_gen is a simple C program that generates the edge list - `network..txt` - used by GEMF as the contact network
```
wget https://raw.githubusercontent.com/nizzaneela/replication/main/src/ba_gen.c
gcc ba_gen.c -o ba_gen
sudo mv ba_gen* /usr/local/bin/
```
