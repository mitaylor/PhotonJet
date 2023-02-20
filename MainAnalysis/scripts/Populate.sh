#!/usr/bin/env bash

histogram() {
    tag=$1

    echo "nohup ./bin/populate configs/populate/preapproval-parallel/populate_${tag}_raw.conf /data/submit/mitay/populate/populate_${tag}_raw.root > log/populate_${tag}_raw_log.txt 2>&1 &"
    echo "nohup ./bin/populate configs/populate/preapproval-parallel/populate_${tag}_bkg.conf /data/submit/mitay/populate/populate_${tag}_bkg.root > log/populate_${tag}_bkg_log.txt 2>&1 &"
}

samples=(pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)
# samples=(pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

for sample in ${samples[@]}; do
    histogram ${sample}

    for syst in wo_ele_rej qcd qcd_gen_iso; do
        histogram ${sample}_${syst}
    done
done

# samples=(aa)

# for sample in ${samples[@]}; do
#     for syst in cent_down cent_up; do
#         histogram ${sample}_${syst}
#     done
# done