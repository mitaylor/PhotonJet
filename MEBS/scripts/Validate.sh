#!/usr/bin/env bash

./bin/validate configs/validate/validate_mebs_jpt_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf temp.root
./bin/validate configs/validate/validate_mebs_dphi_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf temp.root
./bin/validate configs/validate/validate_mebs_eta_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf temp.root
