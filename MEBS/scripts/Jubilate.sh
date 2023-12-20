#!/usr/bin/env bash

# ./bin/jubilate configs/jubilate/jubilate_nominal_eta_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf test.root
# ./bin/jubilate configs/jubilate/jubilate_nominal_dphi_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf test.root
# ./bin/jubilate configs/jubilate/jubilate_nominal_jpt_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf test.root
./bin/jubilate configs/jubilate/jubilate_extra_eta_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf test.root
./bin/jubilate configs/jubilate/jubilate_extra_dphi_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf test.root
./bin/jubilate configs/jubilate/jubilate_extra_jpt_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf test.root