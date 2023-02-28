#!/usr/bin/env bash

arithmetic() {
    tag=$1

    echo ${tag}

    echo "./bin/get_chi_square_iteration_files configs/quantitate/quantitate_${tag}.conf data/preapproval/data_iteration_${tag}.root"
    ./bin/get_chi_square_iteration_files configs/quantitate/quantitate_${tag}.conf data/preapproval/data_iteration_${tag}.root

    echo "./bin/chi_square_iteration configs/chi_square_iteration/chi_square_iteration_${tag}.conf data/preapproval/iteration_${tag}.root"
    ./bin/chi_square_iteration configs/chi_square_iteration/chi_square_iteration_${tag}.conf data/preapproval/iteration_${tag}.root

    echo "./bin/quantitate configs/quantitate/quantitate_${tag}.conf data/preapproval/quantitate_${tag}.root"
    ./bin/quantitate configs/quantitate/quantitate_${tag}.conf data/preapproval/quantitate_${tag}.root
}

nominal() {
    sample=$1

    arithmetic ${sample}
}

systematic() {
    sample=$1
    syst=$2

    arithmetic ${sample}_${syst}
}

# samples=(pp aa pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

# for sample in ${samples[@]}; do
#     nominal $sample

#     for syst in wo_ele_rej qcd qcd_gen_iso loose_purity tight_purity jeu_up jeu_down jer_up stat_matrix prior_down prior_up; do # prior_down prior_up
#         systematic $sample $syst
#     done
# done

samples=(aa)

# for sample in ${samples[@]}; do
#     for syst in cent_up cent_down mebs qcd_mebs qcd_all; do
#         systematic $sample $syst
#     done
# done

for sample in ${samples[@]}; do
    for syst in mebs qcd_mebs; do
        systematic $sample $syst
    done
done
