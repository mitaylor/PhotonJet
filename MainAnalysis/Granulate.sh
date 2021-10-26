#!/usr/bin/env bash

samples=(pp aa pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    # manipulation of indirect systematics
    ./bin/granulate configs/granulate/ran/granulate_${sample}.conf \
        data/ran/granulate_${sample}.root
done
