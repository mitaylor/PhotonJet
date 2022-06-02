#!/usr/bin/env bash

# AANominal: Execute
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_aa.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/AANominalFine0.root
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_aa.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/AANominalFine1.root
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_aa.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/AANominalFine2.root
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_aa.root \
    --DataHistogram aa_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_aa.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/AANominalFine3.root

# AAQCD: Execute
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_aa_qcd.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_0 \
    --ResponseTruth aa_g_0 \
    --ResponseReco aa_r_0 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/AAQCDFine0.root
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_aa_qcd.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_1 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_1 \
    --ResponseTruth aa_g_1 \
    --ResponseReco aa_r_1 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/AAQCDFine1.root
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_aa_qcd.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_2 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_2 \
    --ResponseTruth aa_g_2 \
    --ResponseReco aa_r_2 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/AAQCDFine2.root
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_aa_qcd.root \
    --DataHistogram aa_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_3 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_aa_qcd.root \
    --ResponseHistogram aa_c_3 \
    --ResponseTruth aa_g_3 \
    --ResponseReco aa_r_3 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/AAQCDFine3.root

# PPNominal: Execute
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_pp.root \
    --DataHistogram pp_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_pp.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/PPNominalFine0.root

# PPQCD: Execute
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_pp_qcd.root \
    --DataHistogram pp_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_pp_qcd.root \
    --ResponseHistogram pp_c_0 \
    --ResponseTruth pp_g_0 \
    --ResponseReco pp_r_0 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/PPQCDFine0.root

# PPNominal: Execute
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_pp_smear_50_90.root \
    --DataHistogram pp_smear_50_90_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_pp_smear_50_90.root \
    --ResponseHistogram pp_smear_50_90_c_0 \
    --ResponseTruth pp_smear_50_90_g_0 \
    --ResponseReco pp_smear_50_90_r_0 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/PPSmear90NominalFine0.root

# PPQCD: Execute
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_pp_smear_50_90_qcd.root \
    --DataHistogram pp_smear_50_90_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_pp_smear_50_90_qcd.root \
    --ResponseHistogram pp_smear_50_90_c_0 \
    --ResponseTruth pp_smear_50_90_g_0 \
    --ResponseReco pp_smear_50_90_r_0 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/PPSmear90QCDFine0.root

# PPNominal: Execute
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_pp_smear_30_50.root \
    --DataHistogram pp_smear_30_50_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_pp_smear_30_50.root \
    --ResponseHistogram pp_smear_30_50_c_0 \
    --ResponseTruth pp_smear_30_50_g_0 \
    --ResponseReco pp_smear_30_50_r_0 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/PPSmear50NominalFine0.root

# PPQCD: Execute
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_pp_smear_30_50_qcd.root \
    --DataHistogram pp_smear_30_50_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_pp_smear_30_50_qcd.root \
    --ResponseHistogram pp_smear_30_50_c_0 \
    --ResponseTruth pp_smear_30_50_g_0 \
    --ResponseReco pp_smear_30_50_r_0 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/PPSmear50QCDFine0.root

# PPNominal: Execute
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_pp_smear_10_30.root \
    --DataHistogram pp_smear_10_30_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_pp_smear_10_30.root \
    --ResponseHistogram pp_smear_10_30_c_0 \
    --ResponseTruth pp_smear_10_30_g_0 \
    --ResponseReco pp_smear_10_30_r_0 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/PPSmear30NominalFine0.root

# PPQCD: Execute
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_pp_smear_10_30_qcd.root \
    --DataHistogram pp_smear_10_30_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_pp_smear_10_30_qcd.root \
    --ResponseHistogram pp_smear_10_30_c_0 \
    --ResponseTruth pp_smear_10_30_g_0 \
    --ResponseReco pp_smear_10_30_r_0 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/PPSmear30QCDFine0.root

# PPNominal: Execute
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_pp_smear_0_10.root \
    --DataHistogram pp_smear_0_10_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_pp_smear_0_10.root \
    --ResponseHistogram pp_smear_0_10_c_0 \
    --ResponseTruth pp_smear_0_10_g_0 \
    --ResponseReco pp_smear_0_10_r_0 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/PPSmear10NominalFine0.root

# PPQCD: Execute
./Execute --Data ../../MainAnalysis/data/jet_20_fine/accumulate_pp_smear_0_10_qcd.root \
    --DataHistogram pp_smear_0_10_qcd_nominal_s_pure_raw_sub_pjet_u_dr_sum0_0 \
    --Response ../../MainAnalysis/data/jet_20_fine/vacillate_pp_smear_0_10_qcd.root \
    --ResponseHistogram pp_smear_0_10_c_0 \
    --ResponseTruth pp_smear_0_10_g_0 \
    --ResponseReco pp_smear_0_10_r_0 \
    --PrimaryRecoBins 0,0.005,0.01,0.015,0.02,0.025,0.03,0.035,0.04,0.045,0.05,0.055,0.06,0.065,0.07,0.075,0.08,0.085,0.09,0.0975,0.105,0.1125,0.12,0.1275,0.135,0.1425,0.15,0.165,0.18,0.195,0.21,0.255,0.3 \
    --BinningRecoBins 20,25,30,32.5,35,37.5,40,42.5,45,47.5,50,52.5,55,57.5,60,65,70,75,80,85,90,95,100,110,120,135,150,175,200,225,250 \
    --PrimaryGenBins 0,0.0075,0.015,0.0225,0.03,0.0375,0.045,0.0525,0.06,0.07,0.08,0.09,0.1,0.11,0.12,0.135,0.15,0.175,0.2,0.25,0.3 \
    --BinningGenBins 20,25,30,35,40,45,50,55,60,65,70,75,80,90,100,110,120,200,280 \
    --Output Output/PPSmear10QCDFine0.root