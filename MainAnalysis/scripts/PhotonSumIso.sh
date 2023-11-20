#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/photon_isolation_study configs/populate/serial/populate_${tag}_raw.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/photon_isolation_study_${tag}_raw.root > log/${set}_photon_isolation_study_${tag}_raw.txt 2>&1 &
    ./bin/photon_isolation_study configs/populate/serial/populate_${tag}_bkg.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/photon_isolation_study_${tag}_bkg.root > log/${set}_photon_isolation_study_${tag}_bkg.txt 2>&1 &
}

run_aa() {
    tag=$1

    ./bin/photon_isolation_study configs/populate/serial/populate_${tag}_raw.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/photon_isolation_study_${tag}_raw.root > log/${set}_photon_isolation_study_${tag}_raw.txt 2>&1 &
    ./bin/photon_isolation_study configs/populate/serial/populate_${tag}_bkg.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/photon_isolation_study_${tag}_bkg.root > log/${set}_photon_isolation_study_${tag}_bkg.txt 2>&1 &
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}

    for syst in qcd; do
        run_pp ${sample}_${syst}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}

    for syst in qcd; do
        run_aa ${sample}_${syst}
    done
done