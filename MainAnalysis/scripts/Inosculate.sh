#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    # ./bin/inosculate configs/inosculate/inosculate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/inosculate_${tag}.root > log/${set}_inosculate_${tag}.txt 2>&1 &
    ./bin/inosculate_ele configs/inosculate/inosculate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/inosculate_ele_${tag}.root > log/${set}_inosculate_ele_${tag}.txt 2>&1 &
}

run_aa() {
    tag=$1

    # ./bin/inosculate configs/inosculate/inosculate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/inosculate_${tag}.root > log/${set}_inosculate_${tag}.txt 2>&1 &
    ./bin/inosculate_ele configs/inosculate/inosculate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/inosculate_ele_${tag}.root > log/${set}_inosculate_ele_${tag}.txt 2>&1 &
}

samples=(pp pp_zee)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa aa_zee)

for sample in ${samples[@]}; do
    run_aa ${sample}
done