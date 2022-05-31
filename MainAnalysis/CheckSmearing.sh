#!/usr/bin/env bash

nominal() {
    tag=$1

    ./bin/fabulate_angle configs/fabulate/new/fabulate_${tag}_test.conf \
        data/fabulate_${tag}_smear_test.root
    ./bin/distillate_angle configs/distillate/new/distillate_${tag}_dj_smear_test.conf \
        data/distillate_${tag}_dj_smear_test.root
}

samples=(pp_angle_smear_0_10 pp_angle_smear_10_30 pp_angle_smear_30_50 pp_angle_smear_50_90)

for sample in ${samples[@]}; do
    nominal $sample
done
