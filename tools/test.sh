#!/bin/bash

cd $(dirname $0)
cd ..

echo "Compiling binary..."
make all > /dev/null || exit 1

echo
echo "Running invalid tests..."

for npuzzle_file in $(find puzzles/invalid -type f)
do
    printf "%15s" $(basename $npuzzle_file)
    ./n-puzzle $npuzzle_file >& /dev/null
    if [ $? -eq 0 ]
    then
        echo " - FAIL"
    else
        echo " - PASS"
    fi
done

echo
echo "Running valid tests..."

for npuzzle_file in $(find puzzles/valid -type f)
do
    printf "%15s" $(basename $npuzzle_file)
    ./n-puzzle $npuzzle_file >& /dev/null
    if [ $? -ne -0 ]
    then
        echo " - FAIL"
    else
        echo " - PASS"
    fi
done

echo
echo "Running benchmark tests..."

for npuzzle_file in $(find puzzles/bench -type f)
do
    printf "%15s:" $(basename $npuzzle_file)
    ./n-puzzle $npuzzle_file --bench >& bench.json
    python3 tools/parse_bench.py bench.json
    rm bench.json
done
