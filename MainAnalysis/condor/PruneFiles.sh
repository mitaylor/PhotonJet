#!/usr/bin/env bash

strings=$(grep -rl "Error in <TNetXNGFile::Open>: \[FATAL\] Auth failed" . | grep err | awk -F . 'BEGIN{FS=OFS="."} {$3="*"; print}')

for string in ${strings}; do
    files=($(ls -lt ${string} | awk '{print $9}'))

    for i in ${!files[@]}; do
        if [ $i -gt 1 ]; then
            echo ${files[i]}
        fi
    done
done



# list=${1}

# folders=($(awk -F '/' '{print $2}' ${list}))
# numbers=($(awk -F '.' '{print $4}' ${list}))
# old_files=$(cat ${list})

# cp /tmp/x509up_u168456 x509up_u168456

# old_folder="x"

# for i in ${!folders[@]}; do
#     if [ ${old_folder} != ${folders[i]} ]
#     then
#         if [ ${old_folder} != "x" ]
#         then
#             cd ${old_folder}
#             condor_submit SubmitCondor_${old_folder}.condor
#             cd ..
#         fi

#         echo "${numbers[i]}, ${folders[i]}_${numbers[i]}.conf" > "${folders[i]}/${folders[i]}.list"
#     else
#         echo "${numbers[i]}, ${folders[i]}_${numbers[i]}.conf" >> "${folders[i]}/${folders[i]}.list"
#     fi

#     old_folder=${folders[i]}
# done

# cd ${old_folder}
# condor_submit SubmitCondor_${old_folder}.condor
# cd ..

# for i in ${!old_files[@]}; do
#     rm -f ${old_files[i]}
# done