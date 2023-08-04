#!/usr/bin/env bash


set=$1

./bin/congratulate configs/congratulate/preapproval/congratulate.conf configs/analysis/${set}/analysis_pp.conf temp.root
./bin/congratulate configs/congratulate/preapproval/congratulate_smeared.conf configs/analysis/${set}/analysis_pp.conf temp.root
./bin/ratio configs/congratulate/preapproval/ratio.conf configs/analysis/${set}/analysis_pp.conf temp.root
./bin/congratulate_mean configs/congratulate/preapproval/congratulate_mean.conf configs/analysis/${set}/analysis_pp.conf temp.root
./bin/congratulate_mean configs/congratulate/preapproval/congratulate_mean_zoom.conf configs/analysis/${set}/analysis_pp.conf temp.root