#!/usr/bin/env bash

set=$1

run_pp_theory() {
    tag=$1

    ./bin/compare_unfolding_closure_theory configs/compare_unfolding_closure_gen/compare_unfolding_algorithm_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/compare_unfolding_closure_gen_${tag}.root
}

run_aa_theory() {
    tag=$1

    ./bin/compare_unfolding_closure_theory configs/compare_unfolding_closure_gen/compare_unfolding_algorithm_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/compare_unfolding_closure_gen_${tag}.root
}

run_pp_pythia() {
    tag=$1

    ./bin/compare_unfolding_closure_pythia configs/compare_unfolding_closure_gen/compare_unfolding_algorithm_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/compare_unfolding_closure_gen_${tag}.root
}

run_aa_pythia() {
    tag=$1

    ./bin/compare_unfolding_closure_pythia configs/compare_unfolding_closure_gen/compare_unfolding_algorithm_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/compare_unfolding_closure_gen_${tag}.root
}

samples=(pp)

for sample in ${samples[@]}; do
    for input in pyquen_pp_mc pyquen_pp_flat pyquen_nowide_aa_mc pyquen_nowide_aa_flat pyquen_aa_mc pyquen_aa_flat jewel_pp_mc jewel_pp_flat jewel_norecoil_aa_mc jewel_norecoil_aa_flat jewel_aa_mc jewel_aa_flat; do
        run_pp_theory ${sample}_${input}
    done
    for input in pythia_mc pythia_flat; do
        run_pp_pythia ${sample}_${input}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    for input in pyquen_pp_mc pyquen_pp_flat pyquen_nowide_aa_mc pyquen_nowide_aa_flat pyquen_aa_mc pyquen_aa_flat jewel_pp_mc jewel_pp_flat jewel_norecoil_aa_mc jewel_norecoil_aa_flat jewel_aa_mc jewel_aa_flat; do
        run_aa_theory ${sample}_${input}
    done
    for input in pythia_mebs_nonclosure_mc pythia_mebs_nonclosure_flat pythia_mc pythia_flat; do
        run_aa_pythia ${sample}_${input}
    done
done