#!/usr/bin/env bash

set=$1

./bin/congratulate configs/congratulate/congratulate.conf configs/analysis/${set}/analysis_pp.conf temp.root
./bin/ratio configs/congratulate/ratio.conf configs/analysis/${set}/analysis_pp.conf temp.root
# ./bin/congratulate_mean configs/congratulate/congratulate_mean.conf configs/analysis/${set}/analysis_pp.conf temp.root
# ./bin/congratulate_mean configs/congratulate/congratulate_mean_zoom.conf configs/analysis/${set}/analysis_pp.conf temp.root