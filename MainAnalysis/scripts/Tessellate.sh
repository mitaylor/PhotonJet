#!/usr/bin/env bash

arithmetic() {
    tag=$1

    echo "./bin/tessellate configs/tessellate/preapproval/tessellate_${tag}.conf data/preapproval/tessellate_${tag}.root"
}

nominal() {
    sample=$1

    arithmetic ${sample}
}

systematic() {
    sample=$1
    syst=$2

    arithmetic ${sample}_${syst}
}

samples=(pp aa)

for sample in ${samples[@]}; do
    nominal $sample

    for syst in loose tight qcd; do
        systematic $sample $syst
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    for syst in cent_up cent_down; do
        systematic $sample $syst
    done
done