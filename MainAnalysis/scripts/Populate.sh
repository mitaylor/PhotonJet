#!/usr/bin/env bash

histogram() {
    tag=$1

    echo "nohup ./bin/populate configs/populate/new/jet_20_fine/populate_${tag}_raw.conf data/jet_20_fine/populate_${tag}_raw.root > log/populate_${tag}_raw_jet_20_fine_log.txt 2>&1 &"
    echo "nohup ./bin/populate configs/populate/new/jet_20_fine/populate_${tag}_bkg.conf data/jet_20_fine/populate_${tag}_bkg.root > log/populate_${tag}_bkg_jet_20_fine_log.txt 2>&1 &"
}

samples=(pp pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90 aa)
# samples=(pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    histogram ${sample}

    for syst in wo_ele_rej qcd qcd_gen_iso; do
        histogram ${sample}_${syst}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    for syst in cent_down cent_up; do
        histogram ${sample}_${syst}
    done
done