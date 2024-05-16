#!/usr/bin/env bash

run() {
    tag=$1
    echo $tag
    # ./bin/fabulate configs/fabulate/fabulate_${tag}.conf data/arc/fabulate_${tag}.root
    ./bin/fabulate_angle configs/fabulate/fabulate_${tag}_angle.conf data/arc/fabulate_${tag}_angle.root
    # echo "./bin/fabulate_angle_smeared configs/fabulate/fabulate_${tag}_angle.conf data/arc/fabulate_${tag}_angle.root"
}

samples=(pp aa)

for sample in ${samples[@]}; do
    run ${sample}
done