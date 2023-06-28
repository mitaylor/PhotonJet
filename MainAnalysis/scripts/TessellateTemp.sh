#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    echo "nohup ./bin/tessellate configs/tessellate/preapproval/tessellate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/tessellate_${tag}.root > log/${set}_tessellate_${tag}.txt 2>&1 &"
}

run_aa() {
    tag=$1

    echo "nohup ./bin/tessellate configs/tessellate/preapproval/tessellate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/tessellate_${tag}.root > log/${set}_tessellate_${tag}.txt 2>&1 &"
}

samples=(pp)

for sample in ${samples[@]}; do
    for syst in es; do
        run_pp ${sample}_${syst}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    for syst in es; do
        run_aa ${sample}_${syst}
    done
done