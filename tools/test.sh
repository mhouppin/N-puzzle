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

for npuzzle_file in $(find puzzles/bench -type f)
do
    echo
    for weight in 10.0 8.0 6.0 5.0 4.0 3.0 2.5 2.0 1.5 1.2 1.0
    do
        printf "%15s (weight $weight): " $(basename $npuzzle_file)
        ./n-puzzle $npuzzle_file --verbose --weight=$weight --hole --maxnodes=100000 >& output.txt
        python3 tools/parse_output.py output.txt | tee tmp.txt
        if grep "Unsolved" tmp.txt >& /dev/null; then break; fi
    done
done

rm -f tmp.txt output.txt
