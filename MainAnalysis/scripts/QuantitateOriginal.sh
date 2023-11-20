#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    echo ${tag}

    ./bin/get_iteration_files_original configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/data_iteration_${tag}.root
    ./bin/iteration_original configs/iteration/iteration_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/iteration_${tag}.root
    ./bin/quantitate_original configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/quantitate_${tag}.root
}

run_aa() {
    tag=$1

    echo ${tag}

    ./bin/get_iteration_files_original configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/data_iteration_${tag}.root
    ./bin/iteration_original configs/iteration/iteration_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/iteration_${tag}.root
    ./bin/quantitate_original configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/quantitate_${tag}.root
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}

    for syst in wo_ele_rej qcd qcd_gen_iso loose_purity tight_purity jeu_up jeu_down jer_up; do
        run_pp ${sample}_${syst}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}

    for syst in wo_ele_rej qcd qcd_gen_iso loose_purity tight_purity jeu_up jeu_down jer_up cent_up cent_down; do
        run_aa ${sample}_${syst}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    for syst in mebs qcd_mebs; do
        run_aa ${sample}_${syst}
    done
done