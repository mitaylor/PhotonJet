#!/usr/bin/env bash

set=$1

./bin/theory configs/theory/theory_aa.conf configs/analysis/${set}/analysis_aa.conf temp.root
./bin/theory configs/theory/theory_pp.conf configs/analysis/${set}/analysis_pp.conf temp.root
./bin/theory_ratio configs/theory/theory_ratio.conf configs/analysis/${set}/analysis_aa.conf temp.root