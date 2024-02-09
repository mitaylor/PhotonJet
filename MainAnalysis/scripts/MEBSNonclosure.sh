#!/usr/bin/env bash

set=$1

run_aa() {
    tag=$1

    ./bin/mebs_nonclosure configs/populate/serial/populate_${tag}_raw.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/${set}/populate_${tag}_mebs_nonclosure_raw.root > log/${set}_mebs_nonclosure_${tag}_raw.txt 2>&1 &
    ./bin/mebs_nonclosure configs/populate/serial/populate_${tag}_bkg.conf configs/analysis/${set}/analysis_aa.confdata/arc/${set}/populate_${tag}_mebs_nonclosure_bkg.root > log/${set}_mebs_nonclosure_${tag}_bkg.txt 2>&1 &
}

samples=(aa)

for sample in ${samples[@]}; do
    for syst in qcd; do
        run_aa ${sample}_${syst}
    done
done