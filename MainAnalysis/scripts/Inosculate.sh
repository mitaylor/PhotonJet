#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    # ./bin/inosculate configs/inosculate/preapproval/inosculate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/inosculate_${tag}.root > log/${set}_inosculate_${tag}.txt 2>&1 &
    ./bin/inosculate_ele configs/inosculate/preapproval/inosculate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/inosculate_ele_${tag}.root > log/${set}_inosculate_ele_${tag}.txt 2>&1 &
}

run_aa() {
    tag=$1

    # ./bin/inosculate configs/inosculate/preapproval/inosculate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/inosculate_${tag}.root > log/${set}_inosculate_${tag}.txt 2>&1 &
    ./bin/inosculate_ele configs/inosculate/preapproval/inosculate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/inosculate_ele_${tag}.root > log/${set}_inosculate_ele_${tag}.txt 2>&1 &
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}
done