#!/usr/bin/env bash

samples=(pp aa pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    # manipulation of indirect systematics
    ./bin/data_mc_correction configs/data_mc_correction/data_mc_correction_${sample}.conf \
        data/jet_20/weight_${sample}.root
    ./bin/granulate configs/granulate/ran_jes_on/granulate_${sample}.conf \
        data/jet_20/granulate_${sample}.root
    ./bin/obnubilate configs/obnubilate/ran_jes_on/obnubilate_${sample}.conf \
        data/jet_20/obnubilate_${sample}.root
done

./bin/congratulate configs/congratulate/new/congratulate.conf temp.root
./bin/congratulate configs/congratulate/new/congratulate_smeared.conf temp.root
./bin/congratulate configs/congratulate/new/congratulate_smeared_corrected.conf temp.root