#!/usr/bin/env bash

histogram() {
    tag=$1

    echo "nohup ./bin/populate configs/populate/new/jet_30/populate_${tag}_raw.conf data/jet_30/populate_${tag}_raw.root > log/populate_${tag}_raw_jet_30_log.txt 2>&1 &"
    echo "nohup ./bin/populate configs/populate/new/jet_30/populate_${tag}_bkg.conf data/jet_30/populate_${tag}_bkg.root > log/populate_${tag}_bkg_jet_30_log.txt 2>&1 &"
}

samples=(pp aa)

for sample in ${samples[@]}; do
    histogram ${sample}

    for syst in wo_ele_rej qcd qcd_gen_iso; do
        histogram ${sample}_${syst}
    done
done