# requirements
## external components that require compiling
GEMF, the epidemic simulation software Frahad Sadheh and Futing fan
```
wget https://ece.k-state.edu/netse/software/GEMF%20C%20FUTING.zip

```
## included components that require compiling
ba_gen is a simple C program that generates the edge list - `network..txt` - used by GEMF as the contact network
```
wget https://github.com/nizzaneela/replication/blob/f7f0b0a9bcbc083e5e5044e44d903e9df0f2ebb3/src/ba_gen.c
gcc ba_gen.c -o ba_gen
sudo mv ba_gen* /usr/local/bin/
```
