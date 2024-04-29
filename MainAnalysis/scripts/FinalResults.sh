#!/usr/bin/env bash

set=$1

./bin/final_results_data_ratio_spectra configs/final/final_results_data_ratio_whole.conf configs/analysis/${set}/analysis_aa.conf test.root
./bin/final_results_data_ratio_spectra configs/final/final_results_data_ratio_subsets.conf configs/analysis/${set}/analysis_aa.conf test.root
./bin/final_results_data_ratio_spectra configs/final/final_results_data_spectra_whole.conf configs/analysis/${set}/analysis_aa.conf test.root
./bin/final_results_data_ratio_spectra configs/final/final_results_data_spectra_subsets.conf configs/analysis/${set}/analysis_aa.conf test.root