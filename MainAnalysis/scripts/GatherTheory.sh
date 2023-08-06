#!/usr/bin/env bash


set=$1

run_pp() {
    tag=$1

    ./bin/gather_theory configs/gather_theory/gather_theory_${tag}.conf \
        configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/gather_theory_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/gather_theory configs/gather_theory/gather_theory_${tag}.conf \
        configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/gather_theory_${tag}.root
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}
done

./bin/gather_hybrid configs/gather_theory/gather_hybrid.conf \
    data/preapproval/pho_40/gather_hybrid.root