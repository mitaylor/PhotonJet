#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/compare_photon_pt_spectrum_accumulate configs/test/compare_photon_pt_spectrum_accumulate_pp.conf data/preapproval/${set}/compare_photon_pt_spectrum_accumulate_pp.root
    ./bin/compare_photon_pt_spectrum_populate configs/test/compare_photon_pt_spectrum_populate_pp.conf data/preapproval/${set}/compare_photon_pt_spectrum_populate_pp.root
    ./bin/compare_photon_pt_spectrum configs/test/compare_photon_pt_spectrum_pp_nominal.conf data/preapproval/${set}/compare_photon_pt_spectrum_pp_nominal.root
    # ./bin/compare_photon_pt_spectrum configs/test/compare_photon_pt_spectrum_pp_no_eff.conf data/preapproval/${set}/compare_photon_pt_spectrum_pp_no_eff.root
}

run_aa() {
    tag=$1

    ./bin/compare_photon_pt_spectrum_accumulate configs/test/compare_photon_pt_spectrum_accumulate_aa.conf data/preapproval/${set}/compare_photon_pt_spectrum_accumulate_aa.root
    ./bin/compare_photon_pt_spectrum_populate configs/test/compare_photon_pt_spectrum_populate_aa.conf data/preapproval/${set}/compare_photon_pt_spectrum_populate_aa.root
    ./bin/compare_photon_pt_spectrum configs/test/compare_photon_pt_spectrum_aa_nominal.conf data/preapproval/${set}/compare_photon_pt_spectrum_aa_nominal.root
    # ./bin/compare_photon_pt_spectrum configs/test/compare_photon_pt_spectrum_aa_no_eff.conf data/preapproval/${set}/compare_photon_pt_spectrum_aa_no_eff.root
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}
done