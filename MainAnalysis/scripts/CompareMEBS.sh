#!/usr/bin/env bash

set=$1

run_aa() {
    tag=$1

    ./bin/compare_mebs_jubilate configs/test/compare_mebs_jubilate.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/compare_mebs_jubilate.root
    ./bin/compare_mebs_before_unfolding configs/test/compare_mebs_before_unfolding.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/compare_mebs_before_unfolding.root
    # ./bin/closure configs/closure/closure_aa_mebs.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/closure_aa_mebs.root
    # ./bin/compare_mebs_after_unfolding configs/test/compare_mebs_after_unfolding_data.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/compare_mebs_after_unfolding_data.root
    # ./bin/compare_mebs_after_unfolding configs/test/compare_mebs_after_unfolding_qcd.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/compare_mebs_after_unfolding_qcd.root
}

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}
done