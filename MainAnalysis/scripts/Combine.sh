#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/combine_populate configs/combine_populate/combine_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/populate_${tag}.root
}

run_pp() {
    tag=$1

    ./bin/combine_populate configs/combine_populate/combine_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/populate_${tag}.root
}

samples=(pp_raw pp_bkg pp_qcd_raw pp_qcd_bkg pp_qcd_gen_iso_raw pp_qcd_gen_iso_bkg pp_wo_ele_rej_raw pp_wo_ele_rej_bkg)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(pp_smear_0_10_raw pp_smear_0_10_bkg pp_smear_0_10_qcd_raw pp_smear_0_10_qcd_bkg pp_smear_0_10_qcd_gen_iso_raw pp_smear_0_10_qcd_gen_iso_bkg pp_smear_0_10_wo_ele_rej_raw pp_smear_0_10_wo_ele_rej_bkg)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(pp_smear_10_30_raw pp_smear_10_30_bkg pp_smear_10_30_qcd_raw pp_smear_10_30_qcd_bkg pp_smear_10_30_qcd_gen_iso_raw pp_smear_10_30_qcd_gen_iso_bkg pp_smear_10_30_wo_ele_rej_raw pp_smear_10_30_wo_ele_rej_bkg)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(pp_smear_30_50_raw pp_smear_30_50_bkg pp_smear_30_50_qcd_raw pp_smear_30_50_qcd_bkg pp_smear_30_50_qcd_gen_iso_raw pp_smear_30_50_qcd_gen_iso_bkg pp_smear_30_50_wo_ele_rej_raw pp_smear_30_50_wo_ele_rej_bkg)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(pp_smear_50_90_raw pp_smear_50_90_bkg pp_smear_50_90_qcd_raw pp_smear_50_90_qcd_bkg pp_smear_50_90_qcd_gen_iso_raw pp_smear_50_90_qcd_gen_iso_bkg pp_smear_50_90_wo_ele_rej_raw pp_smear_50_90_wo_ele_rej_bkg)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa_raw aa_bkg aa_qcd_raw aa_qcd_bkg aa_qcd_gen_iso_raw aa_qcd_gen_iso_bkg aa_wo_ele_rej_raw aa_wo_ele_rej_bkg aa_cent_down_raw aa_cent_down_bkg aa_cent_up_raw aa_cent_up_bkg aa_no_eff_raw aa_no_eff_bkg)

for sample in ${samples[@]}; do
    run_aa ${sample}
done

# samples=(aa_mebs_raw aa_mebs_bkg aa_qcd_mebs_raw aa_qcd_mebs_bkg)

# for sample in ${samples[@]}; do
#     run_aa ${sample}
# done