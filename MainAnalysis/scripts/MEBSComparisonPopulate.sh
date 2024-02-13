#!/usr/bin/env bash

set=$1

run_aa() {
    tag=$1

    ./bin/mebs_comparison_populate configs/mebs_comparison/mebs_comparison_populate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/mebs_comparison_populate_${tag}.root
}


samples=(aa_qcd_raw)

for sample in ${samples[@]}; do
    run_aa ${sample}
done