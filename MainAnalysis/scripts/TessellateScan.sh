#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    echo "nohup ./bin/tessellate_squeeze_scan configs/tessellate/scan/tessellate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/tessellate_${tag}.root > log/${set}_tessellate_${tag}.txt 2>&1 &"
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}

    for syst in pp_fine pp_80 pp_85 pp_90 pp_93 pp_95 pp_97 pp_98 pp_99; do
        run_pp ${syst}
    done
done