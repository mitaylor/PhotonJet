#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    echo "nohup ./bin/truth_gen_reco configs/vacillate_dr/preapproval/vacillate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/truth_gen_reco_${tag}.root > log/${set}_truth_gen_reco_${tag}.txt 2>&1 &"
}

run_aa() {
    tag=$1

    echo "nohup ./bin/truth_gen_reco configs/vacillate_dr/preapproval/vacillate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/truth_gen_reco_${tag}.root > log/${set}_truth_gen_reco_${tag}.txt 2>&1 &"
}

samples=(pp pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}
done