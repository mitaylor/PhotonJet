#!/usr/bin/env bash

folder=${1}
output=${2}


find ${folder} -type f > ${output}
sed -i 's/.root/.root \\/g' ${output}
cat ${output}
