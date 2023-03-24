#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/jubilate configs/jubilate/preapproval/jubilate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/jubilate_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/jubilate configs/jubilate/preapproval/jubilate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/jubilate_${tag}.root
}

# samples=(pp_raw pp_qcd_raw aa_mebs_raw aa_qcd_mebs_raw aa_qcd aa_raw) # add aa_qcd_original at some point
samples=(aa_qcd_mebs_raw aa_qcd_raw)

for sample in ${samples[@]}; do
    histogram ${sample}
done

./bin/combine_populate configs/combine_populate/combine_aa_no_eff_raw.conf configs/analysis/pho_40/analysis_aa.conf data/preapproval/pho_40/populate_aa_no_eff_raw.root
./bin/combine_populate configs/combine_populate/combine_aa_no_eff_raw.conf configs/analysis/pho_60/analysis_aa.conf data/preapproval/pho_60/populate_aa_no_eff_raw.root