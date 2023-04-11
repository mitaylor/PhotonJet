#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/manipulate configs/manipulate/preapproval/manipulate_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/preapproval/${set}/manipulate_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/manipulate configs/manipulate/preapproval/manipulate_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/preapproval/${set}/manipulate_${tag}.root
}

# samples=(pp pp_smear_0_10 pp_smear_10_30 pp_smear_30_50 pp_smear_50_90)

# for sample in ${samples[@]}; do
#     run_pp $sample

#     for syst in qcd qcd_gen_iso wo_ele_rej tight_purity loose_purity; do
#         run_pp ${sample}_${syst}
#     done
# done

# samples=(aa)

# for sample in ${samples[@]}; do
#     run_aa $sample

#     for syst in qcd qcd_gen_iso wo_ele_rej tight_purity loose_purity cent_up cent_down; do
#         run_aa ${sample}_${syst}
#     done
# done

# samples=(aa)

for sample in ${samples[@]}; do
    for syst in mebs qcd_mebs; do
        run_aa ${sample}_${syst}
    done
done