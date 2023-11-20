#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    echo "nohup ./bin/truth_gen_reco configs/truth_gen_reco/truth_gen_reco_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/truth_gen_reco_${tag}.root > log/${set}_truth_gen_reco_${tag}.txt 2>&1 &"
}

run_aa() {
    tag=$1

    echo "nohup ./bin/truth_gen_reco configs/truth_gen_reco/truth_gen_reco_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/truth_gen_reco_${tag}.root > log/${set}_truth_gen_reco_${tag}.txt 2>&1 &"
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}
done