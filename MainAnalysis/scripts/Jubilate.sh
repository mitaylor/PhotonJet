#!/usr/bin/env bash

histogram() {
    tag=$1

    ./bin/jubilate configs/jubilate/new/jubilate_${tag}.conf data/jubilate_${tag}_raw.root > log/jubilate_${tag}_raw_log.txt
}

samples=(pp aa aa_qcd_0_10 aa_qcd_0_10_noex aa_nojes aa_noer aa_noeff aa_filtered aa_feff)

for sample in ${samples[@]}; do
    histogram ${sample}
done