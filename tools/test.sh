#!/bin/bash

cd $(dirname $0)
cd ..

echo "Compiling binary..."
test -x ./n-puzzle || make all > /dev/null || exit 1

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
    ./n-puzzle $npuzzle_file --maxnodes=100000 >& /dev/null
    if [ $? -ne -0 ]
    then
        echo " - FAIL"
    else
        echo " - PASS"
    fi
done

echo
echo "Running benchmark tests..."

for npuzzle_size in 3 4 5 6 7 8 10 15 20 25 50 99
do
    echo
    echo "Testing with size $npuzzle_size:"

    for shuffle_its in 5 10 15 20 25 30 40 50 75 100 150 200 300 400 500
    do
        printf " - %3d iters: " $shuffle_its
        python3 tools/npuzzle-gen.py -s -i $shuffle_its $npuzzle_size > puzzle.np
        ./n-puzzle puzzle.np --verbose --max-memory=1024 --bwp >& output.txt
        python3 tools/parse_output.py output.txt | tee tmp.txt
        if grep "Unsolved" tmp.txt >& /dev/null; then break; fi
    done
done

rm -f tmp.txt output.txt puzzle.np
