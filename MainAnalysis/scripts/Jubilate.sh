#!/usr/bin/env bash

histogram() {
    tag=$1

    ./bin/jubilate configs/jubilate/new/jubilate_${tag}.conf data/jubilate_${tag}_raw.root > log/jubilate_${tag}_raw_log.txt
}

samples=(pp pp_qcd aa aa_qcd)

for sample in ${samples[@]}; do
    histogram ${sample}
done