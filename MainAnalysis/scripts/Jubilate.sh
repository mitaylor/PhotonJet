#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/jubilate configs/jubilate/preapproval/jubilate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/jubilate_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/jubilate configs/jubilate/preapproval/jubilate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/jubilate_${tag}.root
}

samples=(pp_raw pp_qcd_raw)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa_raw aa_qcd_raw)

for sample in ${samples[@]}; do
    run_aa ${sample}
done