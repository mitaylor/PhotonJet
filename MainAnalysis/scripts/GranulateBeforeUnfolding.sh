#!/usr/bin/env bash

set=$1

run_aa() {
    tag=$1

    ./bin/granulate_before_unfolding configs/granulate_before_unfolding/granulate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/granulate_before_unfolding_${tag}}.root
}


samples=(aa_mebs_nonclosure)

for sample in ${samples[@]}; do
    run_aa ${sample}
done