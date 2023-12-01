#!/usr/bin/env bash

run() {
    tag=$1
    echo $tag
    ./bin/fabulate_reco configs/fabulate/fabulate_${tag}.conf data/arc/fabulate_reco_${tag}.root
}

samples=(pp aa)

for sample in ${samples[@]}; do
    run ${sample}
done