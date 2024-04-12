#!/usr/bin/env bash


set=$1

run_pp() {
    tag=$1

    ./bin/granulate configs/granulate/granulate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/granulate_${tag}.root
    ./bin/obnubilate configs/obnubilate/obnubilate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/obnubilate_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/granulate configs/granulate/granulate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/granulate_${tag}.root
    ./bin/obnubilate configs/obnubilate/obnubilate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/obnubilate_${tag}.root
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp ${sample}
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa ${sample}
done

./bin/congratulate configs/congratulate/congratulate.conf configs/analysis/${set}/analysis_pp.conf temp.root
./bin/ratio configs/congratulate/ratio.conf configs/analysis/${set}/analysis_pp.conf temp.root