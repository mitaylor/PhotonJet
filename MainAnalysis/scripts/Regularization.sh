#!/usr/bin/env bash

set=$1

run_pp() {
    tag=$1

    ./bin/regularization configs/regularization/regularization_${tag}.conf configs/analysis/${set}/analysis_pp.conf data/arc/${set}/regularization_${tag}.root
}

run_aa() {
    tag=$1

    ./bin/regularization configs/regularization/regularization_${tag}.conf configs/analysis/${set}/analysis_aa.conf data/arc/${set}/regularization_${tag}.root
}

# samples=(pp)

# for sample in ${samples[@]}; do
#     for alg in bayes; do
#         for input in pythia pyquen_pp pyquen_aa pyquen_nowide_aa jewel_pp jewel_aa jewel_norecoil_aa; do
#             for prior in mc flat; do
#                 run_pp ${sample}_${alg}_${input}_${prior}
#             done
#         done
#     done

#     for alg in bayes; do
#         for input in pythia; do
#             for prior in mc flat; do
#                 run_pp ${sample}_qcd_${alg}_${input}_${prior}
#             done
#         done
#     done
# done

# samples=(aa)

# for sample in ${samples[@]}; do
#     for alg in bayes; do
#         for input in pythia pyquen_pp pyquen_aa pyquen_nowide_aa jewel_pp jewel_aa jewel_norecoil_aa; do
#             for prior in mc flat; do
#                 run_aa ${sample}_${alg}_${input}_${prior}
#             done
#         done
#     done

#     for alg in bayes; do
#         for input in pythia; do
#             for prior in mc flat; do
#                 run_aa ${sample}_qcd_${alg}_${input}_${prior}
#             done
#         done
#     done
# done

samples=(pp)

for sample in ${samples[@]}; do
    for alg in svd; do
        for input in pythia pyquen_pp pyquen_aa pyquen_nowide_aa jewel_pp jewel_aa jewel_norecoil_aa; do
            for prior in mc flat; do
                run_pp ${sample}_${alg}_${input}_${prior}
            done
        done
    done

    for alg in svd; do
        for input in pythia; do
            for prior in mc flat; do
                run_pp ${sample}_qcd_${alg}_${input}_${prior}
            done
        done
    done
done

samples=(aa)

for sample in ${samples[@]}; do
    for alg in svd; do
        for input in pythia pyquen_pp pyquen_aa pyquen_nowide_aa jewel_pp jewel_aa jewel_norecoil_aa; do
            for prior in mc flat; do
                run_aa ${sample}_${alg}_${input}_${prior}
            done
        done
    done

    for alg in svd; do
        for input in pythia; do
            for prior in mc flat; do
                run_aa ${sample}_qcd_${alg}_${input}_${prior}
            done
        done
    done
done