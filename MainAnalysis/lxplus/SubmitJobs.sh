#!/usr/bin/env bash

folder=${1}
config_fragment=${2}
output_tag=${3}

cp /tmp/x509up_u117190 x509up_u117190

mkdir -p ${output_tag}
cd ${output_tag}
cp ../../configs/regulate/preapproval-parallel/${config_fragment} .
rm -f *

# split the input files so there are no more than 30 files per job
find ${folder} -type f > ${output_tag}
sed -i 's/^.*\/store/root:\/\/xrootd.cmsaf.mit.edu\/\/store/' ${output_tag}

awk -F "[_.]" -v OFS=", " '{print $(NF-1), $0}' ${output_tag} > ${output_tag}.list

cat ../SubmitCondor.condor | sed "s/__MASTER__/${output_tag}/g" > SubmitCondor.condor
sed -i "s/__CONFIG__/${config_fragment}/g" SubmitCondor.condor

condor_submit SubmitCondor.condor