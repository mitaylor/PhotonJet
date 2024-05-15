#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1
    echo $tag

    ./bin/combine_populate configs/combine_populate/combine_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/populate_${tag}.root
}

run_aa() {
    tag=$1
    echo $tag

    ./bin/combine_populate configs/combine_populate/combine_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/populate_${tag}.root
}

samples=(pp_ex_raw pp_ex_bkg pp_raw pp_bkg pp_qcd_raw pp_qcd_bkg pp_qcd_gen_iso_raw pp_qcd_gen_iso_bkg pp_closure_raw pp_closure_bkg pp_wo_ele_rej_raw pp_wo_ele_rej_bkg pp_er_raw pp_er_bkg pp_es_raw pp_es_bkg)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa_raw aa_bkg aa_qcd_raw aa_qcd_bkg aa_qcd_gen_iso_raw aa_qcd_gen_iso_bkg aa_closure_raw aa_closure_bkg aa_wo_ele_rej_raw aa_wo_ele_rej_bkg aa_cent_down_raw aa_cent_down_bkg aa_cent_up_raw aa_cent_up_bkg aa_no_eff_raw aa_no_eff_bkg aa_no_er_raw aa_no_er_bkg aa_es_raw aa_es_bkg)

for sample in ${samples[@]}; do
    run_aa ${sample}
done

samples=(aa_mebs_raw aa_mebs_bkg)

for sample in ${samples[@]}; do
    run_aa ${sample}
done