#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    echo ${tag}

    ./bin/compare_bayes_kerrors_normalized configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/compare_bayes_kerrors_normalized_${tag}.root
    ./bin/compare_bayes_kerrors_unnormalized configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/compare_bayes_kerrors_unnormalized_${tag}.root
}

run_aa() {
    tag=$1

    echo ${tag}

    ./bin/compare_bayes_kerrors_normalized configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/compare_bayes_kerrors_normalized_${tag}.root
    ./bin/compare_bayes_kerrors_unnormalized configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/compare_bayes_kerrors_unnormalized_${tag}.root
}

samples=(pp pp_qcd)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa aa_qcd aa_mebs_nonclosure aa_qcd_mebs_nonclosure)

for sample in ${samples[@]}; do
    run_aa ${sample}
done