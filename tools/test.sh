#!/usr/bin/env zsh

trap "rm -f tmp.txt output.txt puzzle.np; exit 1" SIGINT

cd $(dirname $0)
cd ..

echo "Compiling binary..."
test -x ./n-puzzle || make all > /dev/null || exit 1

basic_tests_ok=1

echo
echo "Running invalid tests..."

for npuzzle_file in $(find puzzles/invalid -type f | sort)
do
    printf "%15s" $(basename $npuzzle_file)
    ./n-puzzle $npuzzle_file >& /dev/null
    if [ $? -eq 0 ]
    then
        echo " - FAIL"
        basic_tests_ok=0
    else
        echo " - PASS"
    fi
done

echo
echo "Running valid tests..."

for npuzzle_file in $(find puzzles/valid -type f | sort)
do
    printf "%15s" $(basename $npuzzle_file)
    ./n-puzzle $npuzzle_file --max-memory=16 >& /dev/null
    if [ $? -ne 0 ]
    then
        echo " - FAIL"
        basic_tests_ok=0
    else
        echo " - PASS"
    fi
done

if [ $basic_tests_ok -eq 0 ]
then
    exit 1
fi

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
        ./n-puzzle puzzle.np --verbose --max-memory=256 --bwp >& output.txt
        python3 tools/parse_output.py output.txt | tee tmp.txt
        if grep "Unsolved" tmp.txt >& /dev/null; then break; fi
    done
done

echo
echo "Running memory tests..."

for npuzzle_size in 3 4 5 6 7 8 10 15 20 25 50 99
do
    printf " - size %2d: " $npuzzle_size
    python3 tools/npuzzle-gen.py -s -i 50 $npuzzle_size > puzzle.np
    valgrind --log-file=leak_report.txt --leak-check=full --show-leak-kinds=definite,possible ./n-puzzle puzzle.np --verbose --max-memory=32 --bwp >& output.txt
    grep -A1 -F "HEAP SUMMARY:" leak_report.txt | tail -n 1 | awk '{print $6"/"$9 " memleaks, "}' | tr -d '\n'
    grep -F "ERROR SUMMARY:" leak_report.txt | awk '{print $4"/"$7 " memerrors"}'
done

rm -f tmp.txt output.txt puzzle.np leak_report.txt
