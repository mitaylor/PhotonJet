#!/usr/bin/env bash

run() {
    tag=$1
    echo $tag
    # ./bin/distillate configs/distillate/distillate_${tag}_scale.conf data/arc/distillate_${tag}_scale.root
    # ./bin/distillate configs/distillate/distillate_${tag}_angle.conf data/arc/distillate_${tag}_angle.root
    ./bin/distillate_angle configs/distillate/distillate_${tag}_angle_dj.conf data/arc/distillate_${tag}_angle_dj.root
}

samples=(pp aa)

for sample in ${samples[@]}; do
    run ${sample}
done