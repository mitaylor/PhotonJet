#!/usr/bin/env bash

run() {
    tag=$1
    echo $tag
    ./bin/emulate configs/emulate/preapproval/emulate_${tag}.conf data/preapproval/emulate_${tag}.root > log/emulate_${tag}.txt
}

samples=(aa pp aa_extra)

for sample in ${samples[@]}; do
    run ${sample}
done