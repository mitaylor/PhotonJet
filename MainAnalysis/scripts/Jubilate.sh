#!/usr/bin/env bash

histogram() {
    tag=$1

    ./bin/jubilate configs/jubilate/preapproval/jubilate_${tag}.conf data/preapproval/jubilate_${tag}.root > log/jubilate_${tag}_log.txt
}

samples=(pp_raw pp_qcd_raw aa_mebs_raw aa_qcd_mebs_raw aa_raw) # add aa_qcd_original at some point

for sample in ${samples[@]}; do
    histogram ${sample}
done