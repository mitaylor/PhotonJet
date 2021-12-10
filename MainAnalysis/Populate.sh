#!/usr/bin/env bash

histogram() {
    tag=$1

    echo "nohup ./bin/populate configs/populate/ran_jes_on/populate_${tag}_raw.conf data/jes_on/populate_${tag}_raw.root > log/populate_${tag}_raw_jes_on_log.txt 2>&1 &"
    echo "nohup ./bin/populate configs/populate/ran_jes_on/populate_${tag}_bkg.conf data/jes_on/populate_${tag}_bkg.root > log/populate_${tag}_bkg_jes_on_log.txt 2>&1 &"
}

samples=(pp aa pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    histogram ${sample}

    for syst in jeu_down jeu_up wo_ele_rej qcd qcd_gen_iso; do
        histogram ${sample}_${syst}
    done
done