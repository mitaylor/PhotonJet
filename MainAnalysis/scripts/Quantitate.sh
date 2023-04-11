#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    echo ${tag}

    ./bin/get_chi_square_iteration_files configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/data_iteration_${tag}.root
    ./bin/chi_square_iteration configs/chi_square_iteration/chi_square_iteration_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/iteration_${tag}.root
    ./bin/quantitate configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/quantitate_${tag}.root
}

run_aa() {
    tag=$1

    echo ${tag}

    ./bin/get_chi_square_iteration_files configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/data_iteration_${tag}.root
    ./bin/chi_square_iteration configs/chi_square_iteration/chi_square_iteration_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/iteration_${tag}.root
    ./bin/quantitate configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/quantitate_${tag}.root
}

samples=(pp pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    run_pp ${sample}

    for syst in wo_ele_rej qcd qcd_gen_iso loose_purity tight_purity jeu_up jeu_down jer_up stat_matrix prior_down prior_up; do # stat_matrix prior_down prior_up
        run_pp ${sample}_${syst}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}

    for syst in wo_ele_rej qcd qcd_gen_iso loose_purity tight_purity jeu_up jeu_down jer_up cent_up cent_down stat_matrix prior_down prior_up; do # stat_matrix prior_down prior_up
        run_aa ${sample}_${syst}
    done
done

# for sample in ${samples[@]}; do
#     for syst in mebs qcd_mebs; do
#         run_aa ${sample}_${syst}
#     done
# done