#!/usr/bin/env bash

nominal() {
    tag=$1

    ./bin/fabulate_angle configs/fabulate/new/fabulate_${tag}_angle.conf \
        data/fabulate_${tag}_angle.root
    ./bin/distillate_angle configs/distillate/new/distillate_${tag}_angle_dj.conf \
        data/distillate_${tag}_angle.root
}

samples=(pp aa pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    nominal $sample
done
