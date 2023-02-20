#!/usr/bin/env bash

samples=(pp aa pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    # manipulation of indirect systematics
    ./bin/data_mc_correction configs/data_mc_correction/data_mc_correction_${sample}.conf \
        data/preapproval/weight_${sample}.root
    ./bin/granulate configs/granulate/preapproval/granulate_${sample}.conf \
        data/preapproval/granulate_${sample}.root
    ./bin/obnubilate configs/obnubilate/preapproval/obnubilate_${sample}.conf \
        data/preapproval/obnubilate_${sample}.root
done

./bin/congratulate configs/congratulate/preapproval/congratulate.conf temp.root
./bin/congratulate configs/congratulate/preapproval/congratulate_smeared.conf temp.root
./bin/congratulate configs/congratulate/preapproval/congratulate_smeared_corrected.conf temp.root