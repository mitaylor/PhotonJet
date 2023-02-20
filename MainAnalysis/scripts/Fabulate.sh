#!/usr/bin/env bash

histogram() {
    tag=$1
    echo $tag
    # ./bin/fabulate configs/fabulate/preapproval/fabulate_${tag}.conf data/preapproval/fabulate_${tag}.root
    ./bin/fabulate_angle configs/fabulate/preapproval/fabulate_${tag}_angle.conf data/preapproval/fabulate_${tag}_angle.root
}

samples=(pp_smear_0_10_test pp_smear_10_30_test pp_smear_30_50_test pp_smear_50_90_tes)

for sample in ${samples[@]}; do
    histogram ${sample}
done