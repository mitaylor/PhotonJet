#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/granulate configs/granulate/granulate_${tag}.conf \
        configs/analysis/${set}/analysis_pp.conf data/arc/${set}/granulate_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/granulate configs/granulate/granulate_${tag}.conf \
        configs/analysis/${set}/analysis_aa.conf data/arc/${set}/granulate_${tag}.root
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}
done