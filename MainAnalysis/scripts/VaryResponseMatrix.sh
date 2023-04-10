#!/usr/bin/env bash

set=$1

samples=(pp pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    ./bin/vary_response_matrix configs/vary_response_matrix/vary_response_matrix_${sample}.conf \
        configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/vacillate_${sample}_stat_response.root
done

samples=(aa)

for sample in ${samples[@]}; do
    ./bin/vary_response_matrix configs/vary_response_matrix/vary_response_matrix_${sample}.conf \
        configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/vacillate_${sample}_stat_response.root
done