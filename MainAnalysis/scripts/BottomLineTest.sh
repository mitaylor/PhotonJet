#!/usr/bin/env bash

for cent in 0 1 2 3
do
    echo "PbPb Centrality $cent"
    echo

    for iteration in 1 2 3 4 5 6 7 8 9 10 15 20 30 40
    do
        echo "Iteration $iteration"
        ./bin/bottom_line_test configs/test/bottom_line_test_aa.conf configs/analysis/pho_60/analysis_aa.conf test.root $cent $iteration
    done

    echo
    echo
done

echo "PP"
echo

for iteration in 1 2 3 4 5 6 7 8 9 10 15 20 30 40
do
    echo "Iteration $iteration"
    ./bin/bottom_line_test configs/test/bottom_line_test_pp.conf configs/analysis/pho_60/analysis_pp.conf test.root 0 $iteration
done

echo
echo