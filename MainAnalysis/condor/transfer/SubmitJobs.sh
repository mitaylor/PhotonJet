#!/usr/bin/env bash

input=${1}
output=${2}
tag=${3}

cp /tmp/x509up_u168456 x509up_u168456

ls ${input}* > inputs

sed -i 's/^.*\/submit/root:\/\/submit50.mit.edu\//' inputs

inputs=($(cat inputs))

rm inputs

touch ${tag}.list

for i in ${!inputs[@]}; do
    echo "${inputs[i]}, ${output}" >> ${tag}.list
done

cat SubmitCondor.condor | sed "s/__MASTER__/${tag}/g" > ${tag}.condor
condor_submit ${tag}.condor