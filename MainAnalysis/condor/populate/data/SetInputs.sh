#!/usr/bin/env bash

analysis=${1}

# populate_aa_raw
./SubmitJobs.sh ${analysis} \
/data/submit/mitay/photons/condor/regulate_aa_data \
/data/submit/mitay/photons/regulate_aa_mb_data \
"populate_aa_raw_template.conf" \
"populate_aa_raw"