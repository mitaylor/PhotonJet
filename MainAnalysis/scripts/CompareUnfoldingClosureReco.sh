#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/compare_unfolding_closure_reco configs/compare_unfolding_closure_reco/compare_unfolding_closure_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/compare_unfolding_closure_reco_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/compare_unfolding_closure_reco configs/compare_unfolding_closure_reco/compare_unfolding_closure_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/compare_unfolding_closure_reco_${tag}.root
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}

    for input in mebs_nonclosure; do
        run_aa ${sample}_${input}
    done
done