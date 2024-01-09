#!/usr/bin/env bash

./bin/jubilate configs/jubilate/jubilate_fast_ncoll_eta_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf test.root
./bin/jubilate configs/jubilate/jubilate_fast_ncoll_dphi_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf test.root
./bin/jubilate configs/jubilate/jubilate_fast_ncoll_jpt_scan_aa_qcd_raw.conf configs/analysis/pho_60/analysis_aa.conf test.root