#!/usr/bin/env bash

set=$1

nominal=$2
unfolding=$3
mebs=$4

primary_reco_bins="0,0.01,0.015,0.02,0.025,0.03,0.04,0.05,0.06,0.07,0.08,0.1,0.12,0.15,0.2"
binning_reco_bins="20,25,30,35,40,50,60,70,80,100,120,200"
primary_gen_bins="0,0.01,0.016,0.024,0.032,0.04,0.05,0.06,0.08,0.1,0.12,0.15,0.2"
binning_gen_bins="20,30,40,50,60,70,80,100,120,200"

if [ ${nominal} -eq 1 ]
    then
    
    # AANominal: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AANominal0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AANominal1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AANominal2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AANominal3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AANominalMerge.root

    # AAES: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_es.root \
        --DataHistogram aa_es_s_pure_es_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAES0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_es.root \
        --DataHistogram aa_es_s_pure_es_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAES1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_es.root \
        --DataHistogram aa_es_s_pure_es_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAES2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_es.root \
        --DataHistogram aa_es_s_pure_es_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAES3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_es.root \
        --DataHistogram aa_es_s_pure_es_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAESMerge.root

    # AAWithoutEleRej: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_wo_ele_rej.root \
        --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_wo_ele_rej.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAWithoutEleRej0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_wo_ele_rej.root \
        --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_wo_ele_rej.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAWithoutEleRej1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_wo_ele_rej.root \
        --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_wo_ele_rej.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAWithoutEleRej2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_wo_ele_rej.root \
        --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_wo_ele_rej.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAWithoutEleRej3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_wo_ele_rej.root \
        --DataHistogram aa_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_wo_ele_rej.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAWithoutEleRejMerge.root

    # AATightPurity: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_tight_purity.root \
        --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_tight_purity.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AATightPurity0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_tight_purity.root \
        --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_tight_purity.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AATightPurity1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_tight_purity.root \
        --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_tight_purity.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AATightPurity2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_tight_purity.root \
        --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_tight_purity.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AATightPurity3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_tight_purity.root \
        --DataHistogram aa_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_tight_purity.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AATightPurityMerge.root

    # AALoosePurity: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_loose_purity.root \
        --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_loose_purity.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AALoosePurity0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_loose_purity.root \
        --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_loose_purity.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AALoosePurity1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_loose_purity.root \
        --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_loose_purity.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AALoosePurity2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_loose_purity.root \
        --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_loose_purity.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AALoosePurity3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_loose_purity.root \
        --DataHistogram aa_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_loose_purity.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AALoosePurityMerge.root

    # AAQCD: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd.root \
        --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAQCD0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd.root \
        --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAQCD1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd.root \
        --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAQCD2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd.root \
        --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAQCD3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd.root \
        --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAQCDMerge.root

    # AAClosure: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_closure.root \
        --DataHistogram aa_closure_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_closure.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAClosure0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_closure.root \
        --DataHistogram aa_closure_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_closure.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAClosure1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_closure.root \
        --DataHistogram aa_closure_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_closure.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAClosure2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_closure.root \
        --DataHistogram aa_closure_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_closure.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAClosure3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_closure.root \
        --DataHistogram aa_closure_nominal_s_pure_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_closure.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAClosureMerge.root

    # AAQCDGenIso: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_gen_iso.root \
        --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd_gen_iso.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAQCDGenIso0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_gen_iso.root \
        --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd_gen_iso.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAQCDGenIso1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_gen_iso.root \
        --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd_gen_iso.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAQCDGenIso2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_gen_iso.root \
        --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd_gen_iso.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAQCDGenIso3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_gen_iso.root \
        --DataHistogram aa_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd_gen_iso.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAQCDGenIsoMerge.root

    # AAJEUUp: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_jeu_up.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAJEUUp0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_jeu_up.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAJEUUp1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_jeu_up.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAJEUUp2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_jeu_up.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAJEUUp3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_jeu_up.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAJEUUpMerge.root

    # AAJEUDown: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_jeu_down.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAJEUDown0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_jeu_down.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAJEUDown1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_jeu_down.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAJEUDown2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_jeu_down.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAJEUDown3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_jeu_down.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAJEUDownMerge.root

    # AAJERUp: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_jer_up.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAJERUp0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_jer_up.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAJERUp1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_jer_up.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAJERUp2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_jer_up.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAJERUp3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_jer_up.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAJERUpMerge.root

    # AACentUp: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_up.root \
        --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_cent_up.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AACentUp0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_up.root \
        --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_cent_up.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AACentUp1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_up.root \
        --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_cent_up.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AACentUp2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_up.root \
        --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_cent_up.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AACentUp3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_up.root \
        --DataHistogram aa_cent_up_s_pure_cent_up_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_cent_up.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AACentUpMerge.root

    # AACentDown: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_down.root \
        --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_cent_down.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AACentDown0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_down.root \
        --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_cent_down.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AACentDown1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_down.root \
        --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_cent_down.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AACentDown2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_down.root \
        --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_cent_down.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AACentDown3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_cent_down.root \
        --DataHistogram aa_cent_down_s_pure_cent_down_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_cent_down.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AACentDownMerge.root

    # PPNominal: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
        --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/PPNominal0.root

    # PPES: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp_es.root \
        --DataHistogram pp_es_s_pure_es_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/PPES0.root

    # PPWithoutEleRej: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp_wo_ele_rej.root \
        --DataHistogram pp_wo_ele_rej_s_pure_wo_ele_rej_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp_wo_ele_rej.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/PPWithoutEleRej0.root

    # PPTightPurity: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp_tight_purity.root \
        --DataHistogram pp_tight_purity_s_pure_tight_purity_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp_tight_purity.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/PPTightPurity0.root

    # PPLoosePurity: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp_loose_purity.root \
        --DataHistogram pp_loose_purity_s_pure_loose_purity_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp_loose_purity.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/PPLoosePurity0.root

    # PPQCD: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp_qcd.root \
        --DataHistogram pp_qcd_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp_qcd.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/PPQCD0.root

    # PPClosure: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp_closure.root \
        --DataHistogram pp_closure_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp_closure.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/PPClosure0.root

    # PPQCDGenIso: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp_qcd_gen_iso.root \
        --DataHistogram pp_qcd_gen_iso_s_pure_gen_iso_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp_qcd_gen_iso.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/PPQCDGenIso0.root

    # PPJEUUp: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
        --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp_jeu_up.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/PPJEUUp0.root

    # PPJEUDown: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
        --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp_jeu_down.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/PPJEUDown0.root

    # PPJERUp: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
        --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp_jer_up.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/PPJERUp0.root

fi

if [ ${mebs} -eq 1 ]
    then

    # AANominalMEBS: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_mebs.root \
        --DataHistogram aa_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AANominalMEBS0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_mebs.root \
        --DataHistogram aa_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AANominalMEBS1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_mebs.root \
        --DataHistogram aa_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AANominalMEBS2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_mebs.root \
        --DataHistogram aa_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AANominalMEBS3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_mebs.root \
        --DataHistogram aa_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AANominalMEBSMerge.root

    # AAQCDMEBS: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_mebs.root \
        --DataHistogram aa_qcd_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAQCDMEBS0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_mebs.root \
        --DataHistogram aa_qcd_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAQCDMEBS1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_mebs.root \
        --DataHistogram aa_qcd_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAQCDMEBS2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_mebs.root \
        --DataHistogram aa_qcd_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAQCDMEBS3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa_qcd_mebs.root \
        --DataHistogram aa_qcd_nominal_mebs_s_pure_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAQCDMEBSMerge.root

fi

if [ ${unfolding} -eq 1 ]
    then

    # AAPriorUp: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_prior_up.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAPriorUp0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_prior_up.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAPriorUp1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_prior_up.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAPriorUp2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_prior_up.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAPriorUp3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_prior_up.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAPriorUpMerge.root

    # AAPriorDown: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_prior_down.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAPriorDown0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_prior_down.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAPriorDown1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_prior_down.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAPriorDown2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_prior_down.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAPriorDown3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_prior_down.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAPriorDownMerge.root

    # AAVaryResponse: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_stat_response.root \
        --ResponseHistogram aa_c_0 \
        --ResponseTruth aa_g_0 \
        --ResponseReco aa_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAVaryResponse0.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_1 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_stat_response.root \
        --ResponseHistogram aa_c_1 \
        --ResponseTruth aa_g_1 \
        --ResponseReco aa_r_1 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAVaryResponse1.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_2 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_stat_response.root \
        --ResponseHistogram aa_c_2 \
        --ResponseTruth aa_g_2 \
        --ResponseReco aa_r_2 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAVaryResponse2.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_3 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_stat_response.root \
        --ResponseHistogram aa_c_3 \
        --ResponseTruth aa_g_3 \
        --ResponseReco aa_r_3 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAVaryResponse3.root
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_aa.root \
        --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_jpt_merge_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_stat_response.root \
        --ResponseHistogram aa_c_merge_0 \
        --ResponseTruth aa_g_merge_0 \
        --ResponseReco aa_r_merge_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/AAVaryResponseMerge.root

    # PPPriorUp: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
        --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp_prior_up.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/PPPriorUp0.root

    # PPPriorDown: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
        --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp_prior_down.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/PPPriorDown0.root

    # PPVaryResponse: Execute
    ./Execute --Data ../../MainAnalysis/data/arc/${set}/accumulate_pp.root \
        --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_jpt_sum0_0 \
        --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp_stat_response.root \
        --ResponseHistogram pp_c_0 \
        --ResponseTruth pp_g_0 \
        --ResponseReco pp_r_0 \
        --PrimaryRecoBins ${primary_reco_bins} \
        --BinningRecoBins ${binning_reco_bins} \
        --PrimaryGenBins ${primary_gen_bins} \
        --BinningGenBins ${binning_gen_bins} \
        --Output Output/${set}/PPVaryResponse0.root

fi