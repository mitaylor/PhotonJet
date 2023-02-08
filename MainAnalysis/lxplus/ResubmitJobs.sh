#!/usr/bin/env bash

list=${1}

folders=($(awk -F '/' '{print $9}' ${list} | awk -F '.' '{print $1}'))
numbers=($(awk -F '.' '{print $3}' ${list} ))
old_files=$(cat ${list})

# cp /tmp/x509up_u117190 x509up_u117190

# old_folder="x"

# for i in ${!folders[@]}; do
#     if [ ${old_folder} != ${folders[i]} ]
#     then
#         if [ ${old_folder} != "x" ]
#         then
#             cd ${old_folder}
#             condor_submit SubmitCondor.condor
#             cd ..
#         fi

#         echo "${numbers[i]}, ${folders[i]}_${numbers[i]}.conf" >> "${folders[i]}/${folders[i]}.list"
#     else
#         echo "${numbers[i]}, ${folders[i]}_${numbers[i]}.conf" >> "${folders[i]}/${folders[i]}.list"
#     fi

#     old_folder=${folders[i]}
# done

# cd ${old_folder}
# condor_submit SubmitCondor.condor
# cd ..

for i in ${!old_files[@]}; do
    rm ${old_files[i]}
    # echo "${old_files[i]}" >> deleted_err.txt
    rm "/eos/cms/store/group/phys_heavyions/mitaylor/Skims/${folders[i]}_${numbers[i]}.root"
    echo "/eos/cms/store/group/phys_heavyions/mitaylor/Skims/${folders[i]}_${numbers[i]}.root"
done