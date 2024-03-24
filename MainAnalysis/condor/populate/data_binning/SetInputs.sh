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

analyses=(pho_60_binning)

for analysis in ${analyses[@]}; do
    run_aa populate_aa_raw ${analysis}
    run_aa populate_aa_bkg ${analysis}
done