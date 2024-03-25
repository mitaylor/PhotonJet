#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/bottom_line_test configs/bottom_line_test/bottom_line_test_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/bottom_line_test_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/bottom_line_test configs/bottom_line_test/bottom_line_test_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/bottom_line_test_${tag}.root
}

samples=(pp)

for sample in ${samples[@]}; do
    for input in pyquen_pp pyquen_aa jewel_pp jewel_aa pythia_pp; do
        run_pp ${sample}_${input}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    for input in pyquen_pp pyquen_aa jewel_pp jewel_aa pythia_pp; do
        run_aa ${sample}_${input}
    done
done