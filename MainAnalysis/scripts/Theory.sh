#!/usr/bin/env bash

./bin/theory configs/theory/theory_aa.conf configs/analysis/pho_40/analysis_aa.conf temp.root
./bin/theory configs/theory/theory_pp.conf configs/analysis/pho_40/analysis_pp.conf temp.root

./bin/theory configs/theory/pho_40/theory_aa.conf configs/analysis/pho_40/analysis_aa.conf temp.root
./bin/theory configs/theory/pho_40/theory_pp.conf configs/analysis/pho_40/analysis_pp.conf temp.root

./bin/theory configs/theory/theory_aa.conf configs/analysis/pho_40_60/analysis_aa.conf temp.root
./bin/theory configs/theory/theory_pp.conf configs/analysis/pho_40_60/analysis_pp.conf temp.root

./bin/theory configs/theory/theory_aa.conf configs/analysis/pho_60/analysis_aa.conf temp.root
./bin/theory configs/theory/theory_pp.conf configs/analysis/pho_60/analysis_pp.conf temp.root

./bin/theory configs/theory/theory_aa.conf configs/analysis/pho_100/analysis_aa.conf temp.root
./bin/theory configs/theory/theory_pp.conf configs/analysis/pho_100/analysis_pp.conf temp.root
