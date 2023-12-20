#!/usr/bin/env bash

# ./bin/combine_populate configs/combine_populate/combine_nominal_dphi_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf data/populate_scan_dphi_nominal_aa_qcd_raw.root
# ./bin/combine_populate configs/combine_populate/combine_nominal_eta_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf data/populate_scan_eta_nominal_aa_qcd_raw.root
# ./bin/combine_populate configs/combine_populate/combine_nominal_jpt_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf data/populate_scan_jpt_nominal_aa_qcd_raw.root

./bin/combine_populate configs/combine_populate/combine_extra_dphi_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf data/populate_scan_dphi_extra_aa_qcd_raw.root
./bin/combine_populate configs/combine_populate/combine_extra_eta_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf data/populate_scan_eta_extra_aa_qcd_raw.root
./bin/combine_populate configs/combine_populate/combine_extra_jpt_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf data/populate_scan_jpt_extra_aa_qcd_raw.root