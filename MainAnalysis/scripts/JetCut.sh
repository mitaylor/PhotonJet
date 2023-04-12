#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/quantitate_cut_jet_pt configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/jet_cut_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/quantitate_cut_jet_pt configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/jet_cut_${tag}.root
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}
done

./bin/compare_jet_cut configs/jet_cut/jet_cut.conf configs/analysis/${set}/analysis_aa.conf 