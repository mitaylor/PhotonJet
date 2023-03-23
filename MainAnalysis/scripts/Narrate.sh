#!/usr/bin/env bash
set=$1

run_aa() {
    tag=$1

    ./bin/narrate configs/narrate/preapproval/narrate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/narrate_${tag}.root > log/${set}_narrate_${tag}.txt 2>&1 &
}

samples=(aa aa_extra)

for sample in ${samples[@]}; do
    run_aa ${sample}

    for syst in cent_down cent_up; do
        run_aa ${sample}_${syst}
    done
done