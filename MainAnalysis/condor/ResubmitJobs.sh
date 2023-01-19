#!/usr/bin/env bash

list=${1}

folders=($(awk -F '/' '{print $2}' ${list}))
numbers=($(awk -F '.' '{print $4}' ${list}))
old_files=$(cat ${list})

cp /tmp/x509up_u168456 x509up_u168456

old_folder="x"

for i in ${!folders[@]}; do
    if [ ${old_folder} != ${folders[i]} ]
    then
        if [ ${old_folder} != "x" ]
        then
            cd ${old_folder}
            condor_submit SubmitCondor_${old_folder}.condor
            cd ..
        fi

        echo "${numbers[i]}, ${folders[i]}_${numbers[i]}.conf" > "${folders[i]}/${folders[i]}.list"
    else
        echo "${numbers[i]}, ${folders[i]}_${numbers[i]}.conf" >> "${folders[i]}/${folders[i]}.list"
    fi

    old_folder=${folders[i]}
done

cd ${old_folder}
condor_submit SubmitCondor_${old_folder}.condor
cd ..

for i in ${!old_files[@]}; do
    rm -f ${old_files[i]}
done

# # split the input files so there are no more than 30 files per job
# find ${folder} -type f > ${output_tag}
# sed -i 's/^.*\/store/root:\/\/xrootd.cmsaf.mit.edu\/\/store/' ${output_tag}
# split -l ${n_files} --numeric-suffixes=${start} -a 3 ${output_tag} ${output_tag}_
# files=`find . -type f -name "${output_tag}_[0-9][0-9][0-9]" -printf '%f\n'`

# # create all of the config files
# for file in ${files}; do
#     mv ${file} ${file}.conf
#     index=$(get_number ${file})

#     sed -i '1 i\ std::vector<std::string> files =' ${file}.conf
#     sed -i 's/$/ \\/' ${file}.conf # add \ after every line
#     sed -i '${s/\\$//;p;x}' ${file}.conf # delete the final \
#     echo "$(cat ${config_fragment})" >> ${file}.conf # add the rest of the configuration file

#     echo "${index}, ${file}.conf" >> "${output_tag}.list"
# done

# echo "$(cat ${output_tag}.list)"

# cat ../SubmitCondor.condor | sed "s/__MASTER__/${output_tag}/g" > SubmitCondor_${output_tag}.condor
# condor_submit SubmitCondor_${output_tag}.condor