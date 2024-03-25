#!/usr/bin/env bash

set=$1
date=$2

nominal=$3
unfolding=$4
mebs=$5

mkdir -p /data/submit/mitay/unfolding/${date}/Input/Data/${set}/

if [[ $set == "pho_60" ]]; then
    primary_reco_bins="0,0.01,0.02,0.03,0.04,0.05,0.06,0.07,0.08,0.09,0.105,0.12,0.135,0.15,0.18,0.21,0.3"
    binning_reco_bins="20,30,35,40,45,50,55,60,70,80,90,100,120,150,200"
    primary_gen_bins="0,0.015,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2,0.3"
    binning_gen_bins="20,30,40,50,60,70,80,100,120,200"
fi 

if [[ $set == "pho_60_rebin1" ]]; then
    primary_reco_bins="0,0.01,0.02,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2"
    binning_reco_bins="30,40,50,60,70,80,100,120,200"
    primary_gen_bins="0,0.01,0.02,0.03,0.045,0.06,0.08,0.1,0.12,0.15,0.2"
    binning_gen_bins="30,40,50,60,70,80,100,120,200"
fi 

if [[ $set == "pho_60_rebin2" ]]; then
    primary_reco_bins="0,0.01,0.015,0.02,0.025,0.03,0.04,0.05,0.06,0.07,0.08,0.1,0.12,0.15,0.2"
    binning_reco_bins="30,35,40,50,60,70,80,100,120,200"
    primary_gen_bins="0,0.01,0.016,0.024,0.032,0.04,0.05,0.06,0.08,0.1,0.12,0.15,0.2"
    binning_gen_bins="30,40,50,60,70,80,100,120,200"
fi 

if [[ $set == "pho_60_rebin3" ]]; then
    primary_reco_bins="0,0.01,0.015,0.02,0.025,0.03,0.04,0.05,0.06,0.07,0.08,0.1,0.15,0.3"
    binning_reco_bins="30,35,40,45,50,60,70,80,100,200"
    primary_gen_bins="0,0.01,0.016,0.024,0.032,0.04,0.05,0.06,0.08,0.1,0.15,0.3"
    binning_gen_bins="30,40,50,60,70,80,100,200"
fi 

if [[ $set == "pho_60_rebin4" ]]; then
    primary_reco_bins="0,0.01,0.015,0.02,0.025,0.03,0.04,0.05,0.06,0.07,0.08,0.1,0.15"
    binning_reco_bins="30,35,40,45,50,60,70,80,100,200"
    primary_gen_bins="0,0.01,0.016,0.024,0.032,0.04,0.05,0.06,0.08,0.1,0.15"
    binning_gen_bins="30,40,50,60,70,80,100,200"
fi 

if [ ${nominal} -eq 1 ]
    then
    
    # AANominal: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AANominal0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AANominal1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AANominal2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AANominal3.root

    # AAMEBSNonclosure: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/granulate_before_unfolding_aa_mebs_nonclosure.root \
        --DataHistogram aa_mod_aa_qcd_mebs_nonclosure_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAMEBSNonclosure0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/granulate_before_unfolding_aa_mebs_nonclosure.root \
        --DataHistogram aa_mod_aa_qcd_mebs_nonclosure_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAMEBSNonclosure1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/granulate_before_unfolding_aa_mebs_nonclosure.root \
        --DataHistogram aa_mod_aa_qcd_mebs_nonclosure_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAMEBSNonclosure2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/granulate_before_unfolding_aa_mebs_nonclosure.root \
        --DataHistogram aa_mod_aa_qcd_mebs_nonclosure_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAMEBSNonclosure3.root

    # AAES: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_es.root \
        --DataHistogram aa_es_s_pure_es_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAES0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_es.root \
        --DataHistogram aa_es_s_pure_es_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAES1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_es.root \
        --DataHistogram aa_es_s_pure_es_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAES2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_es.root \
        --DataHistogram aa_es_s_pure_es_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAES3.root

    # AAWithoutEleRej: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_wo_ele_rej.root \
        --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_wo_ele_rej.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAWithoutEleRej0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_wo_ele_rej.root \
        --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_wo_ele_rej.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAWithoutEleRej1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_wo_ele_rej.root \
        --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_wo_ele_rej.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAWithoutEleRej2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_wo_ele_rej.root \
        --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_wo_ele_rej.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAWithoutEleRej3.root

    # AATightPurity: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_tight_purity.root \
        --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_tight_purity.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AATightPurity0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_tight_purity.root \
        --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_tight_purity.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AATightPurity1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_tight_purity.root \
        --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_tight_purity.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AATightPurity2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_tight_purity.root \
        --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_tight_purity.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AATightPurity3.root

    # AALoosePurity: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_loose_purity.root \
        --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_loose_purity.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AALoosePurity0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_loose_purity.root \
        --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_loose_purity.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AALoosePurity1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_loose_purity.root \
        --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_loose_purity.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AALoosePurity2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_loose_purity.root \
        --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_loose_purity.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AALoosePurity3.root

    # AAQCD: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd.root \
        --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAQCD0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd.root \
        --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAQCD1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd.root \
        --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAQCD2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd.root \
        --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAQCD3.root

    # AAQCDMEBSNonclosure: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_mebs_nonclosure.root \
        --DataHistogram aa_qcd_mebs_nonclosure_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAQCDMEBSNonclosure0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_mebs_nonclosure.root \
        --DataHistogram aa_qcd_mebs_nonclosure_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAQCDMEBSNonclosure1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_mebs_nonclosure.root \
        --DataHistogram aa_qcd_mebs_nonclosure_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAQCDMEBSNonclosure2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_mebs_nonclosure.root \
        --DataHistogram aa_qcd_mebs_nonclosure_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAQCDMEBSNonclosure3.root

    # AAClosure: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_closure.root \
        --DataHistogram aa_closure_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_a.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAClosure0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_closure.root \
        --DataHistogram aa_closure_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_a.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAClosure1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_closure.root \
        --DataHistogram aa_closure_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_a.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAClosure2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_closure.root \
        --DataHistogram aa_closure_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_closure_a.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAClosure3.root

    # AAQCDGenIso: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_gen_iso.root \
        --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_qcd_gen_iso.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAQCDGenIso0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_gen_iso.root \
        --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_qcd_gen_iso.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAQCDGenIso1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_gen_iso.root \
        --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_qcd_gen_iso.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAQCDGenIso2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_gen_iso.root \
        --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_qcd_gen_iso.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAQCDGenIso3.root

    # AAJEUUp: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_jeu_up.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAJEUUp0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_jeu_up.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAJEUUp1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_jeu_up.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAJEUUp2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_jeu_up.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAJEUUp3.root

    # AAJEUDown: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_jeu_down.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAJEUDown0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_jeu_down.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAJEUDown1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_jeu_down.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAJEUDown2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_jeu_down.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAJEUDown3.root

    # AAJERUp: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_jer_up.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAJERUp0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_jer_up.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAJERUp1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_jer_up.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAJERUp2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_jer_up.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAJERUp3.root

    # AACentUp: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_up.root \
        --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_cent_up.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AACentUp0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_up.root \
        --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_cent_up.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AACentUp1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_up.root \
        --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_cent_up.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AACentUp2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_up.root \
        --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_cent_up.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AACentUp3.root

    # AACentDown: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_down.root \
        --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_cent_down.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AACentDown0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_down.root \
        --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_cent_down.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AACentDown1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_down.root \
        --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_cent_down.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AACentDown2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_down.root \
        --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_cent_down.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AACentDown3.root

    # PPNominal: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
        --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --ResponseTruthEfficiency pp_g_eff_0 \
        --ResponseRecoEfficiency pp_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/PPNominal0.root

    # PPES: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp_es.root \
        --DataHistogram pp_es_s_pure_es_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --ResponseTruthEfficiency pp_g_eff_0 \
        --ResponseRecoEfficiency pp_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/PPES0.root

    # PPWithoutEleRej: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp_wo_ele_rej.root \
        --DataHistogram pp_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp_wo_ele_rej.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --ResponseTruthEfficiency pp_g_eff_0 \
        --ResponseRecoEfficiency pp_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/PPWithoutEleRej0.root

    # PPTightPurity: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp_tight_purity.root \
        --DataHistogram pp_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp_tight_purity.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --ResponseTruthEfficiency pp_g_eff_0 \
        --ResponseRecoEfficiency pp_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/PPTightPurity0.root

    # PPLoosePurity: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp_loose_purity.root \
        --DataHistogram pp_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp_loose_purity.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --ResponseTruthEfficiency pp_g_eff_0 \
        --ResponseRecoEfficiency pp_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/PPLoosePurity0.root

    # PPQCD: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp_qcd.root \
        --DataHistogram pp_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp_qcd.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --ResponseTruthEfficiency pp_g_eff_0 \
        --ResponseRecoEfficiency pp_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/PPQCD0.root

    # PPClosure: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp_closure.root \
        --DataHistogram pp_closure_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp_closure_a.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --ResponseTruthEfficiency pp_g_eff_0 \
        --ResponseRecoEfficiency pp_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/PPClosure0.root

    # PPQCDGenIso: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp_qcd_gen_iso.root \
        --DataHistogram pp_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp_qcd_gen_iso.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --ResponseTruthEfficiency pp_g_eff_0 \
        --ResponseRecoEfficiency pp_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/PPQCDGenIso0.root

    # PPJEUUp: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
        --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp_jeu_up.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --ResponseTruthEfficiency pp_g_eff_0 \
        --ResponseRecoEfficiency pp_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/PPJEUUp0.root

    # PPJEUDown: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
        --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp_jeu_down.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --ResponseTruthEfficiency pp_g_eff_0 \
        --ResponseRecoEfficiency pp_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/PPJEUDown0.root

    # PPJERUp: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
        --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp_jer_up.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --ResponseTruthEfficiency pp_g_eff_0 \
        --ResponseRecoEfficiency pp_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/PPJERUp0.root

fi

if [ ${mebs} -eq 1 ]
    then

    # AANominalMEBS: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_mebs.root \
        --DataHistogram aa_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AANominalMEBS0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_mebs.root \
        --DataHistogram aa_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AANominalMEBS1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_mebs.root \
        --DataHistogram aa_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AANominalMEBS2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_mebs.root \
        --DataHistogram aa_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AANominalMEBS3.root

    # AAQCDMEBS: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_mebs.root \
        --DataHistogram aa_qcd_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAQCDMEBS0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_mebs.root \
        --DataHistogram aa_qcd_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAQCDMEBS1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_mebs.root \
        --DataHistogram aa_qcd_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAQCDMEBS2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_mebs.root \
        --DataHistogram aa_qcd_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAQCDMEBS3.root
fi

if [ ${unfolding} -eq 1 ]
    then

    # AAPriorUp: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_prior_up.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAPriorUp0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_prior_up.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAPriorUp1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_prior_up.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAPriorUp2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_prior_up.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAPriorUp3.root

    # AAPriorDown: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_prior_down.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAPriorDown0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_prior_down.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAPriorDown1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_prior_down.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAPriorDown2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_prior_down.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAPriorDown3.root

    # AAVaryResponse: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_stat_response.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --ResponseTruthEfficiency aa_g_eff_0 \
        --ResponseRecoEfficiency aa_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAVaryResponse0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_stat_response.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --ResponseTruthEfficiency aa_g_eff_1 \
        --ResponseRecoEfficiency aa_r_eff_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAVaryResponse1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_stat_response.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --ResponseTruthEfficiency aa_g_eff_2 \
        --ResponseRecoEfficiency aa_r_eff_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAVaryResponse2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_aa_stat_response.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --ResponseTruthEfficiency aa_g_eff_3 \
        --ResponseRecoEfficiency aa_r_eff_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/AAVaryResponse3.root

    # PPPriorUp: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
        --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp_prior_up.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --ResponseTruthEfficiency pp_g_eff_0 \
        --ResponseRecoEfficiency pp_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/PPPriorUp0.root

    # PPPriorDown: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
        --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp_prior_down.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --ResponseTruthEfficiency pp_g_eff_0 \
        --ResponseRecoEfficiency pp_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/PPPriorDown0.root

    # PPVaryResponse: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
        --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/process_vacillate_pp_stat_response.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --ResponseTruthEfficiency pp_g_eff_0 \
        --ResponseRecoEfficiency pp_r_eff_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output /data/submit/mitay/unfolding/${date}/Input/Data/${set}/PPVaryResponse0.root

fi