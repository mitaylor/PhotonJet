#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/accumulate configs/accumulate/accumulate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/accumulate_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/accumulate configs/accumulate/accumulate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/accumulate_${tag}.root
}

samples=(pp)

for sample in ${samples[@]}; do
    run_pp $sample

    for syst in qcd qcd_gen_iso closure wo_ele_rej tight_purity loose_purity es; do
        run_pp ${sample}_${syst}
    done
done

samples=(pp)

for sample in ${samples[@]}; do
    for syst in er; do
        run_pp ${sample}_${syst}
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    run_aa $sample

    for syst in qcd qcd_gen_iso qcd_mebs_nonclosure closure wo_ele_rej tight_purity loose_purity cent_up cent_down no_er es; do
        run_aa ${sample}_${syst}
    done
done

# samples=(aa)

# for sample in ${samples[@]}; do
#     for syst in mebs qcd_mebs; do
#         run_aa ${sample}_${syst}
#     done
# done