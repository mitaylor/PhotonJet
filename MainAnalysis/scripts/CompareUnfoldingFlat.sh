#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    echo ${tag}

    ./bin/compare_flat configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/compare_flat_${tag}.root
}

run_aa() {
    tag=$1

    echo ${tag}

    ./bin/compare_flat configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/compare_flat_${tag}.root
}

samples=(pp pp_qcd)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa aa_qcd)

for sample in ${samples[@]}; do
    run_aa ${sample}
done