#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/populate configs/populate/preapproval-parallel/populate_${tag}_raw.conf configs/analysis/${set}/analysis_pp.conf /data/submit/mitay/populate/${set}/populate_${tag}_raw.root > log/${set}_populate_${tag}_raw.txt 2>&1 &
    ./bin/populate configs/populate/preapproval-parallel/populate_${tag}_bkg.conf configs/analysis/${set}/analysis_pp.conf /data/submit/mitay/populate/${set}/populate_${tag}_bkg.root > log/${set}_populate_${tag}_bkg.txt 2>&1 &
}

samples=(pp)

for sample in ${samples[@]}; do
    for syst in er es_up es_down; do
        run_pp ${sample}_${syst}
    done
done
