#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <num_procs> <num_successes_required>"
    exit 1
fi

num_procs=$1
num_successes_required=$2

source_dir="./main"

total_failures=0

work(){
    local dir="$1"
    local failures=0
    local successes=0
    cd "$dir" || exit 1
    ba_gen
    while [ "$successes" -lt "$num_successes_required" ]; do
        gemfc_nrm para.txt > log.txt
        local sus=$(tail -n 1 output.txt | awk '{print $6}')
        if [ "$sus" -lt 4999900 ]; then
            ((successes++))
        else
            ((failures++))
        fi
    done
    cd -
    echo "$successes successes in directory $dir, $failures failures"
}

for ((i=1; i<=$num_procs; i++)); do
    dir="${source_dir}_proc$i"
    cp -r "$source_dir" "$dir"
    work "$dir" &
done

wait
((product = num_procs * num_successes_required))
echo "All done!"
echo "Total $product successes"
