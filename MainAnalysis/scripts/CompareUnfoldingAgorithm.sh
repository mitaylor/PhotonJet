#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/compare_unfolding_algorithm configs/compare_unfolding_algorithm/compare_unfolding_algorithm_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/compare_unfolding_algorithm_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/compare_unfolding_algorithm configs/compare_unfolding_algorithm/compare_unfolding_algorithm_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/compare_unfolding_algorithm_${tag}.root
}

samples=(pp)

for sample in ${samples[@]}; do
    for input in pythia pyquen_pp pyquen_aa pyquen_nowide_aa jewel_pp jewel_aa jewel_norecoil_aa; do
        for prior in mc flat; do
            run_pp ${sample}_${input}_${prior}
        done
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    for input in pythia pyquen_pp pyquen_aa pyquen_nowide_aa jewel_pp jewel_aa jewel_norecoil_aa; do
        for prior in mc flat; do
            run_aa ${sample}_${input}_${prior}
        done
    done
done