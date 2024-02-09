#!/usr/bin/env bash

analysis=${1}
input=${2}
mb=${3}
config=${4}
tag=${5}

cp /tmp/x509up_u168456 x509up_u168456

mkdir -p ${analysis}_${tag}
cd ${analysis}_${tag}
rm -f *
cp ../../../../configs/populate/condor/${config} .
cp ../../../../configs/analysis/${analysis}/analysis_aa.conf .

ls ${input}*.root > inputs
ls ${mb}*.root > mbs

awk -F "[_.]" '{print $(NF-1)}' inputs > indices

sed -i 's/^.*\/submit/root:\/\/submit50.mit.edu\//' inputs
sed -i 's/^.*\/submit/root:\/\/submit50.mit.edu\//' mbs

inputs=($(cat inputs))
mbs=($(cat mbs))
indices=($(cat indices))

rm inputs
rm mbs
rm indices

touch ${tag}.list

for i in ${!inputs[@]}; do
    echo "${indices[i]}, ${inputs[i]}, ${mbs[i]}" >> ${tag}.list
done

cat ../SubmitCondor.condor | sed "s/__MASTER__/${tag}/g" > SubmitCondor.condor
sed -i "s/__CONFIG__/${config}/g" SubmitCondor.condor
sed -i "s/__ANALYSIS__/${analysis}/g" SubmitCondor.condor

condor_submit SubmitCondor.condor