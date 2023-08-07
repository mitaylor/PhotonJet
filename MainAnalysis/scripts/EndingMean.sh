#!/usr/bin/env bash


set=$1

run_pp() {
    tag=$1
    ./bin/obnubilate_mean configs/obnubilate/preapproval/obnubilate_${tag}.conf \
        configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/obnubilate_mean_${tag}.root
}

samples=(pp aa) # pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90

for sample in ${samples[@]}; do
    run_pp ${sample}
done

./bin/congratulate_mean configs/congratulate/preapproval/congratulate_mean.conf configs/analysis/${set}/analysis_pp.conf temp.root
./bin/congratulate_mean configs/congratulate/preapproval/congratulate_mean_zoom.conf configs/analysis/${set}/analysis_pp.conf temp.root