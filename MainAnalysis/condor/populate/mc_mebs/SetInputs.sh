#!/usr/bin/env bash

run_aa() {
    tag=$1
    analysis=$2

    ./SubmitJobs.sh ${analysis} \
    /data/submit/mitay/photons/qcd/regulate_aa_qcd \
    /data/submit/mitay/photons/regulate_aa_mb_hydjet_mebs \
    "${tag}_template.conf" \
    "${tag}"
}

analyses=(pho_60_rebin1 pho_60_rebin2 pho_60_rebin3 pho_60_rebin4)

for analysis in ${analyses[@]}; do
    for syst in qcd; do
        run_aa populate_aa_${syst}_raw ${analysis}
        run_aa populate_aa_${syst}_bkg ${analysis}
    done
done