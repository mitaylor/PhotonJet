#!/usr/bin/env bash

arithmetic() {
    tag=$1

    ./bin/hallucinate configs/hallucinate/new/jet_30/hallucinate_${tag}_bkg.conf \
        data/jet_30/populate_${tag}_bkg.root
    ./bin/hallucinate configs/hallucinate/new/jet_30/hallucinate_${tag}_bkg.conf \
        data/jet_30/populate_${tag}_bkg.root
}


systematic() {
    sample=$1
    syst=$2

    arithmetic ${sample}_${syst}
}

samples=(aa_qcd)

for sample in ${samples[@]}; do
    arithmetic $sample

    for syst in gen_iso; do
        systematic $sample $syst
    done
done
