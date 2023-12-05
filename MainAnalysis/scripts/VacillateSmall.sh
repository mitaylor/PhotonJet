#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    echo "nohup ./bin/vacillate_improved configs/vacillate/vacillate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/vacillate_${tag}.root > log/${set}_vacillate_${tag}.txt 2>&1 &"
}

run_aa() {
    tag=$1

    echo "nohup ./bin/vacillate_improved configs/vacillate/vacillate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/vacillate_${tag}.root > log/${set}_vacillate_${tag}.txt 2>&1 &"
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}

    for syst in qcd closure; do
        run_pp ${sample}_${syst}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}

    for syst in qcd closure; do
        run_aa ${sample}_${syst}
    done
done