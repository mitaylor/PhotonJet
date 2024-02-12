#!/usr/bin/env bash

set=$1

run_aa() {
    tag=$1

    ./bin/mebs_comparison configs/mebs_comparison/mebs_comparison_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/mebs_comparison_${tag}.root
}


samples=(aa_qcd_raw)

for sample in ${samples[@]}; do
    run_aa ${sample}
done