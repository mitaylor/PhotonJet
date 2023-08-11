#!/usr/bin/env bash

set=$1
n=$2


./RunSumIsoData.sh ${set} populate_aa_raw ${n}
./RunSumIsoData.sh ${set} populate_aa_bkg ${n}
./RunSumIsoMC.sh ${set} populate_aa_qcd_raw ${n}
./RunSumIsoMC.sh ${set} populate_aa_qcd_bkg ${n}