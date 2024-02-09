#!/usr/bin/env bash

run_aa() {
    tag=$1
    analysis=$2

    ./SubmitJobs.sh ${analysis} \
    /data/submit/mitay/photons/condor/regulate_aa_data \
    /data/submit/mitay/photons/regulate_aa_mb_data \
    "${tag}_template.conf" \
    "${tag}"
}

analyses=(pho_60_rebin1 pho_60_rebin2 pho_60_rebin3 pho_60_rebin4)

for analysis in ${analyses[@]}; do
    run_aa populate_aa_raw ${analysis}
    run_aa populate_aa_bkg ${analysis}
done