#!/usr/bin/env bash

set=$1

run_aa() {
    tag=$1

    ./bin/mebs_comparison_accumulate configs/mebs_comparison/mebs_comparison_accumulate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/mebs_comparison_accumulate_${tag}.root
}


samples=(aa_qcd aa)

for sample in ${samples[@]}; do
    run_aa ${sample}
done