#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <num_procs> <num_successes_required>"
    exit 1
fi

num_procs=$1
num_successes_required=$2

source_dir="./main"

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
            ba_gen
        else
            ((failures++))
        fi
    done
    cd -
    echo "gemfc_nrm: $successes successes in directory $dir, $failures failures"
    echo "gemfc_nrm: directory $dir complete at $(date)"
}

for ((i=1; i<=$num_procs; i++)); do
    dir="${source_dir}_proc$i"
    cp -r "$source_dir/" "$dir"
    work "$dir" &
done

wait

echo "All done!"
