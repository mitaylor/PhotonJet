#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/binning configs/populate/serial/populate_${tag}_raw.conf configs/analysis/${set}/analysis_pp.conf /data/submit/mitay/binning/arc/${set}/binning_${tag}_raw.root > log/${set}_binning_${tag}_raw.txt 2>&1 &
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}
done
