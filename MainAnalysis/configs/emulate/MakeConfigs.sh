#!/usr/bin/env bash

config_name=${1}
qcd_tag=${2}

data_files=`ls /data/submit/mitay/photons/condor/regulate_aa_data_*`
pthat15_files=`ls /data/submit/mitay/photons/condor/${qcd_tag}_15_*`
pthat30_files=`ls /data/submit/mitay/photons/condor/${qcd_tag}_30_*`
pthat50_files=`ls /data/submit/mitay/photons/condor/${qcd_tag}_50_*`
pthat80_files=`ls /data/submit/mitay/photons/condor/${qcd_tag}_80_*`
pthat120_files=`ls /data/submit/mitay/photons/condor/${qcd_tag}_120_*`
pthat170_files=`ls /data/submit/mitay/photons/condor/${qcd_tag}_170_*`

echo "std::vector<std::string> data = \\" >> ${config_name}

for file in ${data_files}; do
    echo "${file} \\" >> ${config_name}
done

echo "" >> ${config_name}


get_number () { 
    s="${1%%.*}"
    echo "${s##*[!0-9]}"
}

folder=${1}
config_fragment=${2}
output_tag=${3}
n_files=${4}
start=${5}

mkdir -p ${output_tag}
cd ${output_tag}
rm -f *
# cp /tmp/x509up_u168456 x509up_u168456

# split the input files so there are no more than 30 files per job
find ${folder} -type f > ${output_tag}
sed -i 's/^.*\/store/root:\/\/xrootd.cmsaf.mit.edu\/\/store/' ${output_tag}
split -l ${n_files} --numeric-suffixes=${start} -a 3 ${output_tag} ${output_tag}_
files=`find . -type f -name "${output_tag}_[0-9][0-9][0-9]" -printf '%f\n'`

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

cat ../SubmitCondor.condor | sed "s/__MASTER__/${output_tag}/g" > SubmitCondor_${output_tag}.condor
condor_submit SubmitCondor_${output_tag}.condor