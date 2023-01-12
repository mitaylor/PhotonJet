#!/usr/bin/env bash

get_number { 
    s="${1%%.*}"
    return "${s##*[!0-9]}"
}

folder=${1}
config_fragment=${2}
output_tag=${3}

mkdir -p ${output_tag}
cd ${output_tag}

# split the input files so there are no more than 30 files per job
files=`find ${folder} -type f -printf '%f\n' > ${output_tag}`

# create all of the config files
for file in ${files}; do
    index=$(get_number ${file})
    echo 'std::vector<std::string> files = \' > ${output_tag}_{index}.conf
    echo "${file}" >> ${output_tag}_{index}.conf
    echo "" >> ${output_tag}_{index}.conf
    echo "$(cat ${config_fragment})" >> ${output_tag}_{index}.conf # add the rest of the configuration file
done

find . -type f -name "${output_tag}_*" -printf '%f\n' > "${output_tag}.list"
sed -i '=' "${output_tag}.list"
sed -i 'N; s/\n/, /' "${output_tag}.list"

echo "$(cat ${output_tag}.list)"

# cat ../SubmitCondor.condor | sed "s/__MASTER__/${output_tag}/g" > SubmitCondor_${output_tag}.condor
# condor_submit SubmitCondor_${output_tag}.condor