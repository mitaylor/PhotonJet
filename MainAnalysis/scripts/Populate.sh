#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/populate_original configs/populate/serial/populate_${tag}_raw.conf configs/analysis/${set}/analysis_pp.conf /data/submit/mitay/populate/arc/${set}/populate_${tag}_raw/populate_${tag}_raw.root > log/${set}_populate_${tag}_raw.txt 2>&1 &
    ./bin/populate_original configs/populate/serial/populate_${tag}_bkg.conf configs/analysis/${set}/analysis_pp.conf /data/submit/mitay/populate/arc/${set}/populate_${tag}_bkg/populate_${tag}_bkg.root > log/${set}_populate_${tag}_bkg.txt 2>&1 &
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}

    for syst in wo_ele_rej qcd qcd_gen_iso er es closure; do
        run_pp ${sample}_${syst}
    done
done

run_aa() {
    tag=$1

    ./bin/populate_original configs/populate/serial/populate_${tag}_raw.conf configs/analysis/${set}/analysis_aa.conf /data/submit/mitay/populate/arc/${set}/populate_${tag}_raw/populate_${tag}_raw.root > log/${set}_populate_${tag}_raw.txt 2>&1 &
    ./bin/populate_original configs/populate/serial/populate_${tag}_bkg.conf configs/analysis/${set}/analysis_aa.conf /data/submit/mitay/populate/arc/${set}/populate_${tag}_bkg/populate_${tag}_bkg.root > log/${set}_populate_${tag}_bkg.txt 2>&1 &
}

samples=(aa)

for sample in ${samples[@]}; do
    for syst in qcd qcd_gen_iso closure; do
        run_aa ${sample}_${syst}
    done
done
