#!/usr/bin/env bash


set=$1

run_pp() {
    tag=$1

    ./bin/choose_iteration_study configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/choose_iteration_${tag}.root
    ./bin/closure configs/closure/closure_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/closure_${tag}.root
    
    ./bin/granulate configs/granulate/preapproval/granulate_${tag}.conf \
        configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/granulate_${tag}.root
    ./bin/obnubilate configs/obnubilate/preapproval/obnubilate_${tag}.conf \
        configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/obnubilate_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/choose_iteration_study configs/quantitate/quantitate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/choose_iteration_${tag}.root
    ./bin/closure configs/closure/closure_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/closure_${tag}.root
    
    ./bin/granulate configs/granulate/preapproval/granulate_${tag}.conf \
        configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/granulate_${tag}.root
    ./bin/obnubilate configs/obnubilate/preapproval/obnubilate_${tag}.conf \
        configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/obnubilate_${tag}.root
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}
done

./bin/congratulate configs/congratulate/preapproval/congratulate.conf configs/analysis/${set}/analysis_pp.conf temp.root
./bin/ratio configs/congratulate/preapproval/ratio.conf configs/analysis/${set}/analysis_pp.conf temp.root