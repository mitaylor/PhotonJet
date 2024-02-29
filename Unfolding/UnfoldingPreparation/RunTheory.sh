#!/usr/bin/env bash

set=$1

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

# AAPythia: Execute
./Execute --Data ../../MainAnalysis/data/arc/${set}/vacillate_aa_closure.root \
    --DataHistogram aa_g_0 \
    --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_closure.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output Theory/${set}/AAPythia0.root
./Execute --Data ../../MainAnalysis/data/arc/${set}/vacillate_aa_closure.root \
    --DataHistogram aa_g_1 \
    --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_closure.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output Theory/${set}/AAPythia1.root
./Execute --Data ../../MainAnalysis/data/arc/${set}/vacillate_aa_closure.root \
    --DataHistogram aa_g_2 \
    --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_closure.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output Theory/${set}/AAPythia2.root
./Execute --Data ../../MainAnalysis/data/arc/${set}/vacillate_aa_closure.root \
    --DataHistogram aa_g_3 \
    --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_closure.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output Theory/${set}/AAPythia3.root

# AAJewel: Execute
./Execute --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tJewelPbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output Theory/${set}/AAJewel3.root

# AAJewelNoRecoil: Execute
./Execute --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tJewelPbPbNoRecoil_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output Theory/${set}/AAJewelNoRecoil3.root
    
# AAPyquen: Execute
./Execute --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tPyquenPbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output Theory/${set}/AAPyquen3.root

# AAPyquenNoWide: Execute
./Execute --Data ../../MainAnalysis/data/arc/${set}/gather_theory_aa.root \
    --DataHistogram tPyquenNoWidePbPb_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output Theory/${set}/AAPyquenNoWide3.root

# PPPythia: Execute
./Execute --Data ../../MainAnalysis/data/arc/${set}/vacillate_pp_closure.root \
    --DataHistogram pp_g_0 \
    --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp_closure.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output Theory/${set}/PPPythia0.root

# PPJewel: Execute
./Execute --Data ../../MainAnalysis/data/arc/${set}/gather_theory_pp.root \
    --DataHistogram tJewelPP_dr_jpt_0 \
    --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp_qcd.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output Theory/${set}/PPJewel0.root

# PPPyquen: Execute
./Execute --Data ../../MainAnalysis/data/arc/${set}/gather_theory_pp.root \
    --DataHistogram tPyquenPP_dr_0 \
    --Response ../../MainAnalysis/data/arc/${set}/vacillate_pp_qcd.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins ${primary_reco_bins} \
    --BinningRecoBins ${binning_reco_bins} \
    --PrimaryGenBins ${primary_gen_bins} \
    --BinningGenBins ${binning_gen_bins} \
    --Output Theory/${set}/PPPyquen0.root