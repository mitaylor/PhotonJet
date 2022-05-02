#!/usr/bin/env bash

histogram() {
    tag=$1

    echo "nohup ./bin/populate configs/populate/new/populate_${tag}_raw.conf data/populate_${tag}_raw.root > log/populate_${tag}_raw_log.txt 2>&1 &"
    echo "nohup ./bin/populate configs/populate/new/populate_${tag}_bkg.conf data/populate_${tag}_bkg.root > log/populate_${tag}_bkg_log.txt 2>&1 &"
}

samples=(pp)

for sample in ${samples[@]}; do
    histogram ${sample}

    for syst in wo_ele_rej qcd qcd_gen_iso; do
        histogram ${sample}_${syst}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    histogram ${sample}

    for syst in wo_ele_rej qcd_0_10 qcd_10_30 qcd_30_50 qcd_50_90 qcd_gen_iso_0_10 qcd_gen_iso_10_30 qcd_gen_iso_30_50 qcd_gen_iso_50_90; do
        histogram ${sample}_${syst}
    done
done