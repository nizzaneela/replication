#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Function to generate Barabasi-Albert graph
void generate_barabasi_albert_graph(int n, int m, FILE *file) {
    int i, j, k, count;
    int batch[m];
    int *edges = malloc(2 * n * m * sizeof(int));
    int ok;

    if (edges == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    // Initialize the graph with a nine node star
    count = 0;
    for (i = 0; i < m; ++i) {
        edges[count] = m;
        count++;
        edges[count] = i;
        count++;
        fprintf(file, "%d %d\n", m, i);
    }
    // Add nodes following the Barabasi-Albert model
    for (i = m+1; i < n; ++i) {
        for (j = 0; j < m; ++j) {
            int target = rand() % count;
            batch[j] = edges[target];
            //repeatedly check each new value unitl it is not a duplicate
            do{
                //start assuming ok
                ok = 1;
                //go through all the values that have already been added to this batch
                for (k = 0; k < j; ++k) {
                    //check for a duplicate
                    if (batch[k] == batch[j]){
                        //not ok
                        ok = 0;
                        //get a repalcement
                        int target = rand() % count;
                        batch[j] = edges[target];
                    }
                }
            } while (!ok);
        }
        for (j = 0; j < m; ++j) {
            edges[count] = i;
            count = count + 1;
            edges[count] = batch[j];
            count = count + 1;
            fprintf(file, "%d %d\n", i, batch[j]);
        }
    }
    free(edges);
}

int main() {
    int n = 5000000; // Number of nodes
    int m = 8;       // Attachment parameter
    FILE *file = fopen("network.txt", "w");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    srand(time(NULL)); // Seed for random number generation

    generate_barabasi_albert_graph(n, m, file);

    fclose(file);

    return 0;
}
