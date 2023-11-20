#!/usr/bin/env bash

set=$1

samples=(pp)

for sample in ${samples[@]}; do
    ./bin/vary_response_matrix configs/vary_response_matrix/vary_response_matrix_${sample}.conf \
        configs/analysis/${set}/analysis_pp.conf data/arc/${set}/vacillate_${sample}_stat_response.root

    ./bin/vary_response_matrix configs/vary_response_matrix/vary_response_matrix_${sample}_weighted.conf \
        configs/analysis/${set}/analysis_pp.conf data/arc/${set}/vacillate_weight_${sample}_stat_response.root
done

samples=(aa)

for sample in ${samples[@]}; do
    ./bin/vary_response_matrix configs/vary_response_matrix/vary_response_matrix_${sample}.conf \
        configs/analysis/${set}/analysis_aa.conf data/arc/${set}/vacillate_${sample}_stat_response.root
    ./bin/vary_response_matrix configs/vary_response_matrix/vary_response_matrix_${sample}_weighted.conf \
        configs/analysis/${set}/analysis_aa.conf data/arc/${set}/vacillate_weight_${sample}_stat_response.root
done