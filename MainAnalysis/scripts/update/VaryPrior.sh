#!/usr/bin/env bash

samples=(pp aa pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    ./bin/vary_prior configs/vary_prior/vary_prior_${sample}_up.conf \
        data/preapproval/vacillate_${sample}_prior_up.root
    ./bin/vary_prior configs/vary_prior/vary_prior_${sample}_down.conf \
        data/preapproval/vacillate_${sample}_prior_down.root
done
