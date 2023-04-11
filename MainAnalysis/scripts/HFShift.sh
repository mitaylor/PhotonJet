#!/usr/bin/env bash

set=$1

run_aa() {
    tag=$1

    ./bin/hf_shift configs/hf_shift/hf_shift_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/hf_shift_${tag}.root > log/${set}_hf_shift_${tag}.txt 2>&1 &
}

samples=(qcd_extra qcd_aod qcd_aod_small)

for sample in ${samples[@]}; do
    run_aa ${sample}
done