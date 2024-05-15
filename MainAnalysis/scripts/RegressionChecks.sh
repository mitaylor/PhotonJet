#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/regression_checks configs/regression_checks/regression_checks_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/regression_checks_${tag}.root > log/${set}_regression_checks_${tag}.txt 2>&1 &
}

run_aa() {
    tag=$1

    ./bin/regression_checks configs/regression_checks/regression_checks_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/regression_checks_${tag}.root > log/${set}_regression_checks_${tag}.txt 2>&1 &
}

samples=(pp_gen_25 pp_reco_25 pp_reco_25_bkg pp_reco_27 pp_reco_30)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa_gen_25 aa_reco_25 aa_reco_25_bkg aa_reco_27 aa_reco_30)

for sample in ${samples[@]}; do
    run_aa ${sample}
done