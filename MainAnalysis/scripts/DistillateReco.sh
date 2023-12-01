#!/usr/bin/env bash

run() {
    tag=$1
    echo $tag
    ./bin/distillate configs/distillate/distillate_reco_${tag}_scale.conf data/arc/distillate_reco_${tag}_scale.root
}

samples=(pp aa)

for sample in ${samples[@]}; do
    run ${sample}
done