#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/manipulate configs/manipulate/manipulate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/manipulate_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/manipulate configs/manipulate/manipulate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/manipulate_${tag}.root
}

samples=(aa)

for sample in ${samples[@]}; do
    run_aa $sample

    for syst in qcd; do
        run_aa ${sample}_${syst}
    done
done