#!/usr/bin/env bash

set=$1

# ./bin/final_results_data_ratio_spectra configs/final/final_results_data_ratio_whole.conf configs/analysis/${set}/analysis_aa.conf test.root
# ./bin/final_results_data_ratio_spectra configs/final/final_results_data_ratio_subsets.conf configs/analysis/${set}/analysis_aa.conf test.root
# ./bin/final_results_data_ratio_spectra configs/final/final_results_data_spectra_whole.conf configs/analysis/${set}/analysis_aa.conf test.root
# ./bin/final_results_data_ratio_spectra configs/final/final_results_data_spectra_subsets.conf configs/analysis/${set}/analysis_aa.conf test.root
# ./bin/final_results_data_means configs/final/final_results_data_means_whole.conf configs/analysis/${set}/analysis_aa.conf test.root
# ./bin/final_results_data_means configs/final/final_results_data_means_subsets.conf configs/analysis/${set}/analysis_aa.conf test.root

./bin/final_results_theory_ratio_spectra configs/final/final_results_theory_spectra_aa_whole.conf configs/analysis/${set}/analysis_aa.conf test.root
./bin/final_results_theory_ratio_spectra configs/final/final_results_theory_spectra_aa_subsets.conf configs/analysis/${set}/analysis_aa.conf test.root
./bin/final_results_theory_ratio_spectra configs/final/final_results_theory_spectra_pp_whole.conf configs/analysis/${set}/analysis_aa.conf test.root
./bin/final_results_theory_ratio_spectra configs/final/final_results_theory_spectra_pp_subsets.conf configs/analysis/${set}/analysis_aa.conf test.root
./bin/final_results_theory_ratio_spectra configs/final/final_results_theory_ratio_whole.conf configs/analysis/${set}/analysis_aa.conf test.root
./bin/final_results_theory_ratio_spectra configs/final/final_results_theory_ratio_subsets.conf configs/analysis/${set}/analysis_aa.conf test.root