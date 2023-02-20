#!/usr/bin/env bash

histogram() {
    tag=$1
    echo $tag
    ./bin/distillate configs/distillate/preapproval/distillate_${tag}_scale.conf data/preapproval/distillate_${tag}_scale.root
    ./bin/distillate_angle configs/distillate/preapproval/distillate_${tag}_angle_dj.conf data/preapproval/distillate_${tag}_angle_dj.root
}

samples=(pp_smear_0_10)

for sample in ${samples[@]}; do
    histogram ${sample}
done