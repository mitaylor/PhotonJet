#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/photon_pt_spectrum configs/populate/serial/populate_${tag}_raw.conf configs/analysis/${set}/analysis_pp.conf /data/submit/mitay/spectrum/${set}/populate_${tag}_raw.root > log/${set}_spectrum_populate_${tag}_raw.txt 2>&1 &
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}

    for syst in qcd er qcd_er; do
        run_pp ${sample}_${syst}
    done
done
