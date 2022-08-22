#!/usr/bin/env bash

histogram() {
    tag=$1
    echo $tag
    ./bin/fabulate configs/fabulate/preapproval/fabulate_${tag}.conf data/preapproval/fabulate_${tag}.root
    ./bin/fabulate_angle configs/fabulate/preapproval/fabulate_${tag}_angle.conf data/preapproval/fabulate_${tag}_angle.root
}

samples=(aa pp pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    histogram ${sample}
done