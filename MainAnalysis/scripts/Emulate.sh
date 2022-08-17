#!/usr/bin/env bash

histogram() {
    tag=$1
    echo $tag
    ./bin/emulate configs/emulate/preapproval/emulate_${tag}.conf data/preapproval/emulate_${tag}.root > log/emulate_${tag}.txt
}

samples=(aa pp aa_extra)

for sample in ${samples[@]}; do
    histogram ${sample}
done