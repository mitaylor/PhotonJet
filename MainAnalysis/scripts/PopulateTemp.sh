#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    mkdir -p /data/submit/mitay/populate/arc/${set}/populate_${tag}_raw/
    mkdir -p /data/submit/mitay/populate/arc/${set}/populate_${tag}_bkg/

    ./bin/populate_original configs/populate/serial/populate_${tag}_raw.conf configs/analysis/${set}/analysis_pp.conf /data/submit/mitay/populate/arc/${set}/populate_${tag}_raw/populate_${tag}_raw.root > log/${set}_populate_${tag}_raw.txt 2>&1 &
    ./bin/populate_original configs/populate/serial/populate_${tag}_bkg.conf configs/analysis/${set}/analysis_pp.conf /data/submit/mitay/populate/arc/${set}/populate_${tag}_bkg/populate_${tag}_bkg.root > log/${set}_populate_${tag}_bkg.txt 2>&1 &
}

samples=(pp)

for sample in ${samples[@]}; do
    for syst in ex qcd_ex; do
        run_pp ${sample}_${syst}
    done
done
