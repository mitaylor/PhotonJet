#!/usr/bin/env bash

histogram() {
    tag=$1

    ./bin/jubilate configs/jubilate/preapproval/jubilate_${tag}_raw.conf data/preapproval/jubilate_${tag}_raw.root > log/jubilate_${tag}_raw_log.txt
}

samples=(pp pp_qcd aa aa_qcd) # add aa_qcd_original at some point

for sample in ${samples[@]}; do
    histogram ${sample}
done