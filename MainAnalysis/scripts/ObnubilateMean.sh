#!/usr/bin/env bash


set=$1

run_pp() {
    tag=$1
    ./bin/obnubilate_mean configs/obnubilate/obnubilate_mean_${tag}.conf \
        configs/analysis/${set}/analysis_pp.conf data/arc/${set}/obnubilate_mean_${tag}.root
}

samples=(pp aa)

for sample in ${samples[@]}; do
    run_pp ${sample}
done