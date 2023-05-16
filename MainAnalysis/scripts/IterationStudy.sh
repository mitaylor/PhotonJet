#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    echo ${tag}

    ./bin/iteration_study configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_pp.conf temp.root
}

run_aa() {
    tag=$1

    echo ${tag}

    ./bin/iteration_study configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_aa.conf temp.root
}

samples=(pp pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    run_pp ${sample}

    for syst in qcd; do
        run_pp ${sample}_${syst}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}

    for syst in qcd; do
        run_aa ${sample}_${syst}
    done
done

samples=(aa)