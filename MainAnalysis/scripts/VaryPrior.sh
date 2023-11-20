#!/usr/bin/env bash

set=$1

samples=(pp)

for sample in ${samples[@]}; do
    ./bin/vary_prior configs/vary_prior/vary_prior_${sample}_up.conf \
        configs/analysis/${set}/analysis_pp.conf data/arc/${set}/vacillate_${sample}_prior_up.root
    ./bin/vary_prior configs/vary_prior/vary_prior_${sample}_down.conf \
        configs/analysis/${set}/analysis_pp.conf data/arc/${set}/vacillate_${sample}_prior_down.root
done

samples=(aa)

for sample in ${samples[@]}; do
    ./bin/vary_prior configs/vary_prior/vary_prior_${sample}_up.conf \
        configs/analysis/${set}/analysis_aa.conf data/arc/${set}/vacillate_${sample}_prior_up.root
    ./bin/vary_prior configs/vary_prior/vary_prior_${sample}_down.conf \
        configs/analysis/${set}/analysis_aa.conf data/arc/${set}/vacillate_${sample}_prior_down.root
done