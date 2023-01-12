#!/usr/bin/env bash

folder=${1}
config_fragment=${2}
output_tag=${3}
n_files=${4}

mkdir -p ${output_tag}
cd ${output_tag}

# split the input files so there are no more than 30 files per job
find ${folder} -type f > ${output_tag}
split -l ${n_files} -d -a 3 ${output_tag} ${output_tag}
files=`find . -type f -name "${output_tag}???"`

# create all of the config files
for file in ${files}; do
    sed -i '1 i\ std::vector<std::string> files =' ${file}
    sed -i 's/$/ \\/' ${file} # add \ after every line
    sed -i '${s/\\$//;p;x}' ${file} # delete the final \

    echo "$(cat ${config_fragment})" >> ${file} # add the rest of the configuration file
done

find . -type f -name "${output_tag}???" -printf '%f\n' > "${output_tag}.list"
sed -i '=' "${output_tag}.list"
sed -i 'N; s/\n/, /' "${output_tag}.list"

echo "$(cat ${output_tag}.list)"

cd ..
cat SubmitCondor.condor | sed "s/__MASTER__/${output_tag}/g" > SubmitCondor_${output_tag}.condor
condor_submit SubmitCondor_${output_tag}.condor