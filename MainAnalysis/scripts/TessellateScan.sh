#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    echo "nohup ./bin/tessellate configs/tessellate/preapproval/tessellate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/tessellate_${tag}.root > log/${set}_tessellate_${tag}.txt 2>&1 &"
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}

    for syst in p1 p5 p10 p50 n1 n5 n10 n50; do
        run_pp ${sample}_offset_${syst}
    done
done