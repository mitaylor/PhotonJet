#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    echo "nohup ./bin/tessellate_area configs/tessellate/preapproval/tessellate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/tessellate_area_${tag}.root > log/${set}_tessellate_area_${tag}.txt 2>&1 &"
}

run_aa() {
    tag=$1

    echo "nohup ./bin/tessellate_area configs/tessellate/preapproval/tessellate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/tessellate_area_${tag}.root > log/${set}_tessellate_area_${tag}.txt 2>&1 &"
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}
done