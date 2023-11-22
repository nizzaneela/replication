# requirements
## external components that require compiling
GEMF, the epidemic simulation software Frahad Sadheh and Futing Fan
```
wget https://ece.k-state.edu/netse/software/GEMF%20C%20FUTING.zip
unzip GEMF\ C\ FUTING.zip
cd GEMF\ C\ FUTING/c/



```
## included components that require compiling
ba_gen is a simple C program that generates the edge list - `network..txt` - used by GEMF as the contact network
```
wget https://raw.githubusercontent.com/nizzaneela/replication/main/src/ba_gen.c
gcc ba_gen.c -o ba_gen
sudo mv ba_gen* /usr/local/bin/
```
