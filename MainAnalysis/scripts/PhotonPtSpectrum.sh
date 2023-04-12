#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/photon_pt_spectrum configs/photon_pt_spectrum/preapproval/photon_pt_spectrum_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/photon_pt_spectrum_${tag}.root > log/${set}_photon_pt_spectrum_${tag}.txt 2>&1 &
}

run_aa() {
    tag=$1

    ./bin/photon_pt_spectrum configs/photon_pt_spectrum/preapproval/photon_pt_spectrum_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/photon_pt_spectrum_${tag}.root > log/${set}_photon_pt_spectrum_${tag}.txt 2>&1 &
}

samples=(pp_data_no_eff pp_data pp_qcd)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa_data_no_eff aa_data aa_qcd)

for sample in ${samples[@]}; do
    run_aa ${sample}
done