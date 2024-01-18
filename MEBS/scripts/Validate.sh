#!/usr/bin/env bash

./bin/validate configs/validate/validate_vz_jpt_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf temp.root
./bin/validate configs/validate/validate_vz_dphi_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf temp.root
./bin/validate configs/validate/validate_vz_eta_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf temp.root
