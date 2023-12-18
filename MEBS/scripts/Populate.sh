#!/usr/bin/env bash

set=$1

./bin/populate_dphi_scan_no_weight configs/populate/serial/populate_aa_qcd_pthat30_raw.conf configs/analysis/${set}/analysis_aa.conf /data/submit/mitay/populate/arc/${set}/populate_dphi_scan_aa_qcd_pthat30_raw.root > log/${set}_populate_dphi_scan_aa_qcd_pthat30_raw.txt 2>&1 &
./bin/populate_eta_scan_no_weight configs/populate/serial/populate_aa_qcd_pthat30_raw.conf configs/analysis/${set}/analysis_aa.conf /data/submit/mitay/populate/arc/${set}/populate_eta_scan_aa_qcd_pthat30_raw.root > log/${set}_populate_eta_scan_aa_qcd_pthat30_raw.txt 2>&1 &
./bin/populate_jpt_scan_no_weight configs/populate/serial/populate_aa_qcd_pthat30_raw.conf configs/analysis/${set}/analysis_aa.conf /data/submit/mitay/populate/arc/${set}/populate_jpt_scan_aa_qcd_pthat30_raw.root > log/${set}_populate_jpt_scan_aa_qcd_pthat30_raw.txt 2>&1 &