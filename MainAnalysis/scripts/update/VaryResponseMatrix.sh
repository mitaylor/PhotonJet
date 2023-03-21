#!/usr/bin/env bash

samples=(pp aa pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    ./bin/vary_response_matrix configs/vary_response_matrix/vary_response_matrix_${sample}.conf \
        data/preapproval/vacillate_${sample}_stat_response.root
done
