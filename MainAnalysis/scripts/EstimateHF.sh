#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/estimate_hf configs/estimate_hf/estimate_hf_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/estimate_hf_${tag}.root
}

samples=(pp_data pp_qcd)

for sample in ${samples[@]}; do
    run_pp ${sample}
done