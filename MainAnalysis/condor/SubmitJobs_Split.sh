#!/usr/bin/env bash

get_number () { 
    s="${1%%.*}"
    echo "${s##*[!0-9]}"
}

folder=${1}
config_fragment=${2}
output_tag=${3}
n_files=${4}

mkdir -p ${output_tag}
cd ${output_tag}
rm *

# split the input files so there are no more than 30 files per job
find ${folder} -type f -printf '%f\n' > ${output_tag}
split -l ${n_files} -d -a 3 ${output_tag} ${output_tag}
files=`find . -type f -printf '%f\n' -name ${output_tag}???`

# create all of the config files
for file in ${files}; do
    mv ${file} ${file}.conf
    index=$(get_number ${file})

    sed -i '1 i\ std::vector<std::string> files =' ${file}.conf
    sed -i 's/$/ \\/' ${file}.conf # add \ after every line
    sed -i '${s/\\$//;p;x}' ${file}.conf # delete the final \
    echo "$(cat ${config_fragment})" >> ${file}.conf # add the rest of the configuration file

    echo "${index}, ${file}.conf" >> "${output_tag}.list"
done


echo "$(cat ${output_tag}.list)"

# sed -e '$ ! s/$/,/g' lstfiles.txt | tr -d '\n'

# cat ../SubmitCondor_Split.condor | sed "s/__MASTER__/${output_tag}/g" > SubmitCondor_${output_tag}.condor
# condor_submit SubmitCondor_${output_tag}.condor