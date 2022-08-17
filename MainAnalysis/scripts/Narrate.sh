#!/usr/bin/env bash

histogram() {
    tag=$1

    ./bin/narrate configs/narrate/preapproval/narrate_${tag}.conf data/preapproval/narrate_${tag}.root > log/narrate_${tag}.txt
}

samples=(aa paper_aa aa_extra paper_aa_extra)

for sample in ${samples[@]}; do
    histogram ${sample}

    for syst in cent_down cent_up; do
        histogram ${sample}_${syst}
    done
done