#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    echo ${tag}

    ./bin/get_iteration_files configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/data_iteration_${tag}.root
    ./bin/iteration configs/iteration/iteration_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/iteration_${tag}.root
    ./bin/quantitate configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/quantitate_${tag}.root
}

run_aa() {
    tag=$1

    echo ${tag}

    ./bin/get_iteration_files configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/data_iteration_${tag}.root
    ./bin/iteration configs/iteration/iteration_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/iteration_${tag}.root
    ./bin/quantitate configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/quantitate_${tag}.root
}

samples=(pp pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    run_pp ${sample}

    for syst in stat_matrix prior_down prior_up; do
        run_pp ${sample}_${syst}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}

    for syst in stat_matrix prior_down prior_up; do
        run_aa ${sample}_${syst}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    for syst in mebs qcd_mebs; do
        run_aa ${sample}_${syst}
    done
done