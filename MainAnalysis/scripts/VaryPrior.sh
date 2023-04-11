#!/usr/bin/env bash

set=$1

samples=(pp pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    ./bin/vary_prior configs/vary_prior/vary_prior_${sample}_up.conf \
        configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/vacillate_${sample}_prior_up.root
    ./bin/vary_prior configs/vary_prior/vary_prior_${sample}_down.conf \
        configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/vacillate_${sample}_prior_down.root
done

samples=(aa)

for sample in ${samples[@]}; do
    ./bin/vary_prior configs/vary_prior/vary_prior_${sample}_up.conf \
        configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/vacillate_${sample}_prior_up.root
    ./bin/vary_prior configs/vary_prior/vary_prior_${sample}_down.conf \
        configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/vacillate_${sample}_prior_down.root
done